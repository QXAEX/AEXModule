#include "../h/Thread.h"
#include <thread>
#include <mutex>

static std::mutex mtx;

struct controll_thread_call_info {
    LPTHREAD_START_ROUTINE callback;
    PThread_info info;
    PThread thread;
};
// �߳̿��ƺ���
static void controll_thread_call(controll_thread_call_info* info) {
    HANDLE id = CreateThread(nullptr, 0, info->callback, info->info, 0, nullptr);
    if (id) {
        SetThreadPriority(id, THREAD_PRIORITY_LOWEST);// �����߳����ȼ�, ���
        WaitForSingleObject(id, INFINITE);// �ȴ��߳̽���
        CloseHandle(id);
        info->info->interval[2]--;
        delete info;
    }
};
static void controll_thread(PThread thread) {
    while (thread->is_running) {
        std::lock_guard<std::mutex> lock(mtx); // ��ס�Թ�����Դ�ķ���
        for (auto& info : thread->thread_info_list) {
            if (info.type == Thread_type::THREAD_TYPE_LOOP && 
                System::GetRunTime() - info.interval[1] >= info.interval[0] && info.interval[2] == NULL) {
                info.interval[1] = System::GetRunTime() + info.interval[0];
                info.interval[2]++;
                info.none1 = CreateThread(
                    nullptr, // Ĭ�ϰ�ȫ����
                    0,       // Ĭ�϶�ջ��С
                    (LPTHREAD_START_ROUTINE)controll_thread_call, // �̺߳���
                    new controll_thread_call_info({ (LPTHREAD_START_ROUTINE)info.callback, &info, thread }),  // ���ݵ�ǰ�̶߳���
                    0,       // Ĭ�ϴ�����־
                    nullptr  // �߳� ID
                );
                // �����߳�ִ�лص�
                SetThreadPriority(info.none1, THREAD_PRIORITY_LOWEST);// �����߳����ȼ�, ���

                
            }
        }
        Sleep(thread->loop_interval); // ����ѭ�����
    }
    thread->timer = nullptr;
}

Thread::Thread(int loop_interval) {
    this->is_running = true;
    this->loop_interval = loop_interval;
    this->timer = CreateThread(
        nullptr, // Ĭ�ϰ�ȫ����
        0,       // Ĭ�϶�ջ��С
        (LPTHREAD_START_ROUTINE)controll_thread, // �̺߳���
        this,    // ���ݵ�ǰ����
        0,       // Ĭ�ϴ�����־
        nullptr  // �߳� ID
    );
    if (this->timer)SetThreadPriority(this->timer, THREAD_PRIORITY_LOWEST); // �����߳����ȼ�, ���
}

Thread::~Thread() {
    this->is_running = false; // �����߳�ֹͣ��־
    if (this->timer != nullptr) {
        WaitForSingleObject(this->timer, INFINITE); // �ȴ��߳̽���
        CloseHandle(this->timer); // �رվ��
    }
}

bool __stdcall Thread::add_thread(Byteset code, Thread_callback callback, Thread_type type, __int64 interval) {
    Thread_info info(code, callback, type, interval);
    if (info.callback == nullptr) return false;
    // ����߳��Ƿ��Ѵ���
    for (const auto& existing_info : this->thread_info_list) {
        if (existing_info.code == info.code) return false;
    }
    info.interval[1] = System::GetRunTime();
    info.interval[2] = NULL;
    this->thread_info_list.push_back(info);

    // ����ǵ����̣߳���������
    if (info.type == Thread_type::THREAD_TYPE_ONE_SHOT) {
        this->start_thread(code);
    }

    return true;
}

bool __stdcall Thread::remove_thread(Byteset code) {
    for (auto it = this->thread_info_list.begin(); it != this->thread_info_list.end(); ++it) {
        if (it->code == code) {
            this->thread_info_list.erase(it);
            return true;
        }
    }
    return false;
}

bool __stdcall Thread::start_thread(Byteset code, bool join) {
    std::lock_guard<std::mutex> lock(mtx); // ��ס�Թ�����Դ�ķ���
    for (auto& info : thread_info_list) {
        if (info.code == code && info.status == Thread_status::THREAD_STATUS_NONE) {
            info.status = Thread_status::THREAD_STATUS_RUNNING;
            info.none1 = CreateThread(
                nullptr, // Ĭ�ϰ�ȫ����
                0,       // Ĭ�϶�ջ��С
                (LPTHREAD_START_ROUTINE)info.callback, // �̺߳���
                &info,   // �����߳���Ϣ
                0,       // Ĭ�ϴ�����־
                nullptr  // �߳� ID
            );
            if(info.none1)SetThreadPriority(info.none1, THREAD_PRIORITY_HIGHEST); // �����߳����ȼ�, �������߳�

            if (info.none1 != nullptr) {
                if (join) {
                    WaitForSingleObject(info.none1, INFINITE); // �ȴ��߳̽���
                    CloseHandle(info.none1); // �رվ��
                }
                return true;
            }
        }
    }
    return false;
}

bool __stdcall Thread::stop_thread(Byteset code) {
    std::lock_guard<std::mutex> lock(mtx); // ��ס�Թ�����Դ�ķ���
    for (auto& info : thread_info_list) {
        if (info.code == code && info.status == Thread_status::THREAD_STATUS_RUNNING) {
            TerminateThread(info.none1, 0); // ǿ�ƽ����߳�
            CloseHandle(info.none1); // �رվ��
            info.status = Thread_status::THREAD_STATUS_NONE; // ����״̬
            return true;
        }
    }
    return false;
}
