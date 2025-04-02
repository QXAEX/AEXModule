#include "../h/Thread.h"
#include <chrono>
typedef struct _THREAD_INFO {
    DWORD code;//线程标识
    THREAD_CALLBACK callback;//线程回调函数
    THREAD_TYPE type;//线程类型
    __int64 interval;//线程间隔时间
    HANDLE hThread;//线程句柄
    bool isRunning;//线程是否运行
    bool isJoin;//线程是否等待
    bool isStop;//线程是否停止
} THREAD_INFO, * PTHREAD_INFO;

static std::mutex mtx;
static std::map<DWORD, THREAD_INFO> threads;

DWORD WINAPI Thread::ThreadProc(LPVOID lpParam) {
    THREAD_INFO* pInfo = static_cast<THREAD_INFO*>(lpParam);
    do {
        try {
            if (pInfo->callback) pInfo->callback(reinterpret_cast<PThread>(pInfo));
        }
        catch (...) {
            return THREAD_ERROR;
        }
        if (pInfo->type == THREAD_TYPE_LOOP) std::this_thread::sleep_for(std::chrono::milliseconds(pInfo->interval));
    } while (pInfo->type == THREAD_TYPE_LOOP && !pInfo->isStop);
    {
        std::lock_guard<std::mutex> lock(mtx);
        pInfo->isRunning = false;
        pInfo->isStop = true;
    }
    return THREAD_OK;
}

Thread::~Thread() {
    wait();
}

THREAD_CODE __stdcall Thread::add(DWORD code, THREAD_CALLBACK callback, THREAD_TYPE type, __int64 interval) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (threads.find(code) != threads.end()) return THREAD_EXIST;
        THREAD_INFO info = { code, callback, type, interval, nullptr, false, false, false };
        threads[code] = info;
    }
    if (type == THREAD_TYPE::THREAD_TYPE_ONE_SHOT) return start(code, false);
    else return THREAD_OK;
}

THREAD_CODE __stdcall Thread::remove(DWORD code) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = threads.find(code);
    if (it == threads.end()) return THREAD_NOT_FOUND;
    if (it->second.isRunning) return THREAD_RUNNING;
    if (it->second.hThread) CloseHandle(it->second.hThread);
    threads.erase(it);
    return THREAD_OK;
}

THREAD_CODE __stdcall Thread::start(DWORD code, bool join) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = threads.find(code);
    if (it == threads.end()) return THREAD_NOT_FOUND;
    THREAD_INFO& info = it->second;
    if (info.isRunning) return THREAD_RUNNING;
    info.isStop = false;
    info.isJoin = join;
    info.hThread = CreateThread(nullptr, 0, ThreadProc, &info, 0, nullptr);
    if (!info.hThread) return THREAD_ERROR;
    info.isRunning = true;
    return THREAD_OK;
}

THREAD_CODE __stdcall Thread::stop(DWORD code) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = threads.find(code);
    if (it == threads.end()) return THREAD_NOT_FOUND;
    it->second.isStop = true;
    return THREAD_OK;
}

THREAD_CODE __stdcall Thread::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    std::vector<std::pair<DWORD, HANDLE>> handlePairs;
    for (auto& pair : threads) if (pair.second.isRunning && pair.second.hThread) handlePairs.emplace_back(pair.first, pair.second.hThread);
    lock.unlock();
    for (auto& hp : handlePairs) {
        DWORD result = WaitForSingleObject(hp.second, INFINITE);
        if (result == WAIT_FAILED) return THREAD_ERROR;
        std::lock_guard<std::mutex> lock(mtx);
        if (auto it = threads.find(hp.first); it != threads.end()) {
            it->second.isRunning = false;
            CloseHandle(it->second.hThread);
            it->second.hThread = nullptr;
        }
    }
    return THREAD_OK;
}

THREAD_CODE __stdcall Thread::wait(DWORD code) {
    HANDLE hThread = nullptr;
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = threads.find(code);
        if (it == threads.end()) return THREAD_NOT_FOUND;
        if (!it->second.isRunning) return THREAD_STOPPED;
        hThread = it->second.hThread;
    }
    if (hThread) {
        DWORD result = WaitForSingleObject(hThread, INFINITE);
        if (result == WAIT_FAILED) return THREAD_ERROR;
        std::lock_guard<std::mutex> lock(mtx);
        if (auto it = threads.find(code); it != threads.end()) {
            it->second.isRunning = false;
            CloseHandle(it->second.hThread);
            it->second.hThread = nullptr;
        }
    }
    return THREAD_OK;
}

