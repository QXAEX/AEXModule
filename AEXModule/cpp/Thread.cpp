#include "../h/Thread.h"
#include <thread>
#include <mutex>

static std::mutex mtx;

struct controll_thread_call_info {
    LPTHREAD_START_ROUTINE callback;
    PThread_info info;
    PThread thread;
};
// 线程控制函数
static void controll_thread_call(controll_thread_call_info* info) {
    HANDLE id = CreateThread(nullptr, 0, info->callback, info->info, 0, nullptr);
    if (id) {
        SetThreadPriority(id, THREAD_PRIORITY_LOWEST);// 设置线程优先级, 最低
        WaitForSingleObject(id, INFINITE);// 等待线程结束
        CloseHandle(id);
        info->info->interval[2]--;
        delete info;
    }
};
static void controll_thread(PThread thread) {
    while (thread->is_running) {
        std::lock_guard<std::mutex> lock(mtx); // 锁住对共享资源的访问
        for (auto& info : thread->thread_info_list) {
            if (info.type == Thread_type::THREAD_TYPE_LOOP && 
                System::GetRunTime() - info.interval[1] >= info.interval[0] && info.interval[2] == NULL) {
                info.interval[1] = System::GetRunTime() + info.interval[0];
                info.interval[2]++;
                info.none1 = CreateThread(
                    nullptr, // 默认安全属性
                    0,       // 默认堆栈大小
                    (LPTHREAD_START_ROUTINE)controll_thread_call, // 线程函数
                    new controll_thread_call_info({ (LPTHREAD_START_ROUTINE)info.callback, &info, thread }),  // 传递当前线程对象
                    0,       // 默认创建标志
                    nullptr  // 线程 ID
                );
                // 创建线程执行回调
                SetThreadPriority(info.none1, THREAD_PRIORITY_LOWEST);// 设置线程优先级, 最低

                
            }
        }
        Sleep(thread->loop_interval); // 控制循环间隔
    }
    thread->timer = nullptr;
}

Thread::Thread(int loop_interval) {
    this->is_running = true;
    this->loop_interval = loop_interval;
    this->timer = CreateThread(
        nullptr, // 默认安全属性
        0,       // 默认堆栈大小
        (LPTHREAD_START_ROUTINE)controll_thread, // 线程函数
        this,    // 传递当前对象
        0,       // 默认创建标志
        nullptr  // 线程 ID
    );
    if (this->timer)SetThreadPriority(this->timer, THREAD_PRIORITY_LOWEST); // 设置线程优先级, 最低
}

Thread::~Thread() {
    this->is_running = false; // 设置线程停止标志
    if (this->timer != nullptr) {
        WaitForSingleObject(this->timer, INFINITE); // 等待线程结束
        CloseHandle(this->timer); // 关闭句柄
    }
}

bool __stdcall Thread::add_thread(Byteset code, Thread_callback callback, Thread_type type, __int64 interval) {
    Thread_info info(code, callback, type, interval);
    if (info.callback == nullptr) return false;
    // 检查线程是否已存在
    for (const auto& existing_info : this->thread_info_list) {
        if (existing_info.code == info.code) return false;
    }
    info.interval[1] = System::GetRunTime();
    info.interval[2] = NULL;
    this->thread_info_list.push_back(info);

    // 如果是单次线程，立即启动
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
    std::lock_guard<std::mutex> lock(mtx); // 锁住对共享资源的访问
    for (auto& info : thread_info_list) {
        if (info.code == code && info.status == Thread_status::THREAD_STATUS_NONE) {
            info.status = Thread_status::THREAD_STATUS_RUNNING;
            info.none1 = CreateThread(
                nullptr, // 默认安全属性
                0,       // 默认堆栈大小
                (LPTHREAD_START_ROUTINE)info.callback, // 线程函数
                &info,   // 传递线程信息
                0,       // 默认创建标志
                nullptr  // 线程 ID
            );
            if(info.none1)SetThreadPriority(info.none1, THREAD_PRIORITY_HIGHEST); // 设置线程优先级, 高于主线程

            if (info.none1 != nullptr) {
                if (join) {
                    WaitForSingleObject(info.none1, INFINITE); // 等待线程结束
                    CloseHandle(info.none1); // 关闭句柄
                }
                return true;
            }
        }
    }
    return false;
}

bool __stdcall Thread::stop_thread(Byteset code) {
    std::lock_guard<std::mutex> lock(mtx); // 锁住对共享资源的访问
    for (auto& info : thread_info_list) {
        if (info.code == code && info.status == Thread_status::THREAD_STATUS_RUNNING) {
            TerminateThread(info.none1, 0); // 强制结束线程
            CloseHandle(info.none1); // 关闭句柄
            info.status = Thread_status::THREAD_STATUS_NONE; // 重置状态
            return true;
        }
    }
    return false;
}
