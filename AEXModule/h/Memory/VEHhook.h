#pragma once
#include <functional>
#include "./R3.h"
namespace MemoryVEHHook {
    // 操作类型
    enum TYPE {
        INT3,                  // int3异常断点
        INT3_HOOKJUMP,         // int3 HOOK_JUMP模式
        HARDWARE_BREAKPOINT,   // 硬件断点
        HARDWARE_HOOKJUMP      // 硬件HOOK_JUMP模式
    };
    // 断点类型(硬断有效)
    //enum BRAKPOINT_TYPE {
    //    T_EXECUTE,               // 执行断点
    //    T_WRITE,                  // 数据写入断点
    //    T_IO_READ_WRITE,       // I/0读写断点
    //    T_READ_WRITE             // 读写断点
    //};
    //错误码
    enum ERROR_CODE {
        ERROR_NULL,            // 成功
        ADDRESS_INVALID, // 地址无效
        TOKEN_INVALID,   // 标识符无效
        TYPE_INVALID,    // 类型无效
        LENGTH_INVALID,  // 长度无效
        CALLBACK_INVALID, // 回调函数无效
        HARDWARE_HOOKJUMP_LIMIT, //硬件断点数量达到上限
    };
    // VEHCallBack回调函数类型
    typedef std::function<void WINAPI(
        MemoryVEHHook::TYPE type,// 操作类型
        PEXCEPTION_POINTERS ExceptionInfo// 异常信息
    )> VEHCallBack;
    //VEHhook类
    class VEHhook {
    public:
        // 安装VEHhook
        BOOL install();
        // 卸载VEHhook
        BOOL uninstall() const;
        /*
        * 添加软件断点
        * @param token 标识符
        * @param address 断点地址
        * @param newAddress 触发跳转地址
        * @param type 断点类型
        * @param callBack 回调函数
        * @param data 新地址数据 (用于触发断点后跳入的新地址优先执行的汇编代码)
        * @return 返回错误码
        */
        MemoryVEHHook::ERROR_CODE Add(int token, PVOID address, PVOID newAddress, MemoryVEHHook::TYPE type, MemoryVEHHook::VEHCallBack callBack, Byteset data = {});
        /*
        * 移除硬件断点
        * @param token 标识符
        * @return 返回错误码
        */
        MemoryVEHHook::ERROR_CODE Remove(int token);
        MemoryR3::R3* R3();
    private:
        PVOID exceptionHandler; // 异常处理函数地址
        static LONG NTAPI ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo); // 异常处理函数
    private:
        enum FNC_TRANSITIION {
            NONE, ADD, REMOVE
        };
        static MemoryVEHHook::ERROR_CODE INT3(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition);
        static MemoryVEHHook::ERROR_CODE INT3_HOOKJUMP(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition);
        static MemoryVEHHook::ERROR_CODE HARDWARE_BREAKPOINT(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition);
        static  MemoryVEHHook::ERROR_CODE HARDWARE_HOOKJUMP(PEXCEPTION_POINTERS ExceptionInfo, PVOID pInfo, FNC_TRANSITIION fnc_transition);
    };
};