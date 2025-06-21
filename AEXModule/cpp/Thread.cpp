#include "../h/Thread.h"
#include <chrono>

DWORD WINAPI Thread::ThreadProc(LPVOID lpParam) {
    THREAD_INFO* pInfo = static_cast<THREAD_INFO*>(lpParam);
    do {
        try {
            if (pInfo->isForceStop) {
                return THREAD_OK;
            }
            pInfo->isStop = false;
            pInfo->isRunning = true;
            if (pInfo->callback) pInfo->callback(pInfo->pThread);
            pInfo->isRunning = false;
            pInfo->isStop = true;
        }
        catch (...) {
            return THREAD_ERROR;
        }
        if (pInfo->type == THREAD_TYPE_LOOP) {
            std::this_thread::sleep_for(std::chrono::milliseconds(pInfo->interval));
            pInfo->isStop = false;
        }
    } while (pInfo->type == THREAD_TYPE_LOOP && !pInfo->isStop);
    //{
    //    std::lock_guard<std::mutex> lock(*pInfo->mtx);
    //}
    return THREAD_OK;
}

Thread::Thread()
{
    this->add(-404, [&](PThread pThread) {
        std::vector<DWORD> removeCodes;//已删除线程的code
        while (!this->isExit) {
            for (DWORD code : removeThreads) {
                auto it = threads.find(code);
                if (it == threads.end()) continue;
                if (!it->second.isRunning && it->second.isStop && it->second.isForceStop && it->second.hThread) {
                    CloseHandle(it->second.hThread);
                    threads.erase(it);
                    removeCodes.push_back(code);
                }
            }
            if (removeCodes.size()) {
                std::lock_guard<std::mutex> lock(this->mtx);
                for (DWORD code : removeCodes) {
                    for (DWORD _code : removeThreads) {
                        for (DWORD i = 0; i < removeThreads.size(); i++) {
                            if (_code == code) {
                                removeThreads.erase(removeThreads.begin() + i);
                                break;
                            }
                        }
                    }
                }
                removeCodes.clear();
            }
            Sleep(1000);
        }
        });
}

Thread::~Thread() {
    this->isExit = true;
    wait();
}

THREAD_CODE __stdcall Thread::add(DWORD code, THREAD_CALLBACK callback, THREAD_TYPE type, __int64 interval) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (threads.find(code) != threads.end()) return THREAD_EXIST;
        THREAD_INFO info = { code, callback, type, interval, nullptr, false, false, false, &this->mtx };
        threads[code] = info;
    }
    if (type == THREAD_TYPE::THREAD_TYPE_ONE_SHOT) return start(code, false);
    else return THREAD_OK;
}

THREAD_CODE __stdcall Thread::remove(DWORD code, bool isForce) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = threads.find(code);
    if (it == threads.end()) return THREAD_NOT_FOUND;
    it->second.isForceStop = true;
    if (it->second.isRunning) {
        if (isForce) {
            this->removeThreads.push_back(code);
            return THREAD_OK;
        }
        return THREAD_RUNNING;
    }
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
    info.isForceStop = false;
    info.isJoin = join;
    info.pThread = this;
    info.hThread = CreateThread(nullptr, 0, ThreadProc, &info, 0, nullptr);
    if (!info.hThread) return THREAD_ERROR;
    return THREAD_OK;
}

THREAD_CODE __stdcall Thread::stop(DWORD code) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = threads.find(code);
    if (it == threads.end()) return THREAD_NOT_FOUND;
    if (it->second.hThread) {
        CloseHandle(it->second.hThread);
        it->second.hThread = nullptr;
    }
    it->second.isStop = true;
    it->second.isForceStop = false;
    it->second.isRunning = false;
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

