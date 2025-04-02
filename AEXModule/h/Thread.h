#pragma once
#include <windows.h>
#include <functional>
#include <map>
#include <mutex>
#include "Byteset.h"
#include "System.h"

class Thread;
typedef Thread* PThread;

typedef std::function<void(PThread)> THREAD_CALLBACK;

enum THREAD_STATUS {
    THREAD_STATUS_NONE,//无状态
    THREAD_STATUS_RUNNING,//线程正在运行
    THREAD_STATUS_STOPPED,//线程已停止
};

enum THREAD_TYPE {
    THREAD_TYPE_NONE,//无操作
    THREAD_TYPE_ONE_SHOT,//立即执行一次
    THREAD_TYPE_LOOP,//循环执行
};

enum THREAD_CODE {
    THREAD_OK,//线程操作成功
    THREAD_EXIST,//线程已存在
    THREAD_NOT_FOUND,//线程不存在
    THREAD_RUNNING,//线程正在运行
    THREAD_STOPPED,//线程已停止
    THREAD_ERROR,//线程错误
};

class Thread {
public:
    virtual ~Thread();
    /*
    * @brief 添加线程
    * @param code 线程代码
    * @param callback 线程回调函数
    * @param type 线程类型,默认为THREAD_TYPE_ONE_SHOT（立即执行一次）
    * @param interval 线程间隔时间,单位毫秒,仅在type为THREAD_TYPE_LOOP时有效,默认为0
    * @return 线程状态
    */
    static THREAD_CODE WINAPI add(DWORD code, THREAD_CALLBACK callback, THREAD_TYPE type = THREAD_TYPE_ONE_SHOT, __int64 interval = 0);
    /*
    * @brief 移除线程
    * @param code 线程代码
    * @return 线程状态
    */
    static THREAD_CODE WINAPI remove(DWORD code);
    /*
    * @brief 启动线程
    * @param code 线程代码
    * @param join 是否等待线程结束,默认为false
    * @return 线程状态
    */
    static THREAD_CODE WINAPI start(DWORD code, bool join = false);
    /*
    * @brief 停止线程
    * @param code 线程代码
    * @return 线程状态
    */
    static THREAD_CODE WINAPI stop(DWORD code);
    /*
    * @brief 等待所有线程
    * @return 线程状态
    */
    static THREAD_CODE WINAPI wait();
    /*
    * @brief 等待指定线程
    * @param code 线程代码
    * @return 线程状态
    */
    static THREAD_CODE WINAPI wait(DWORD code);
private:
    static DWORD WINAPI ThreadProc(LPVOID lpParam);
};
