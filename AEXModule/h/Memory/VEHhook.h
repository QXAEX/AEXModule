#pragma once
#include <functional>
#include "./R3.h"
namespace MemoryVEHHook {
    // ��������
    enum TYPE {
        INT3,                  // int3�쳣�ϵ�
        INT3_HOOKJUMP,         // int3 HOOK_JUMPģʽ
        HARDWARE_BREAKPOINT,   // Ӳ���ϵ�
        HARDWARE_HOOKJUMP      // Ӳ��HOOK_JUMPģʽ
    };
    // �ϵ�����(Ӳ����Ч)
    //enum BRAKPOINT_TYPE {
    //    T_EXECUTE,               // ִ�жϵ�
    //    T_WRITE,                  // ����д��ϵ�
    //    T_IO_READ_WRITE,       // I/0��д�ϵ�
    //    T_READ_WRITE             // ��д�ϵ�
    //};
    //������
    enum ERROR_CODE {
        ERROR_NULL,            // �ɹ�
        ADDRESS_INVALID, // ��ַ��Ч
        TOKEN_INVALID,   // ��ʶ����Ч
        TYPE_INVALID,    // ������Ч
        LENGTH_INVALID,  // ������Ч
        CALLBACK_INVALID, // �ص�������Ч
        HARDWARE_HOOKJUMP_LIMIT, //Ӳ���ϵ������ﵽ����
    };
    // VEHCallBack�ص���������
    typedef std::function<void WINAPI(
        MemoryVEHHook::TYPE type,// ��������
        PEXCEPTION_POINTERS ExceptionInfo// �쳣��Ϣ
    )> VEHCallBack;
    //VEHhook��
    class VEHhook {
    public:
        // ��װVEHhook
        BOOL install();
        // ж��VEHhook
        BOOL uninstall() const;
        /*
        * �������ϵ�
        * @param token ��ʶ��
        * @param address �ϵ��ַ
        * @param newAddress ������ת��ַ
        * @param type �ϵ�����
        * @param callBack �ص�����
        * @param data �µ�ַ���� (���ڴ����ϵ��������µ�ַ����ִ�еĻ�����)
        * @return ���ش�����
        */
        MemoryVEHHook::ERROR_CODE Add(int token, PVOID address, PVOID newAddress, MemoryVEHHook::TYPE type, MemoryVEHHook::VEHCallBack callBack, Byteset data = {});
        /*
        * �Ƴ�Ӳ���ϵ�
        * @param token ��ʶ��
        * @return ���ش�����
        */
        MemoryVEHHook::ERROR_CODE Remove(int token);
        MemoryR3::R3* R3();
    private:
        PVOID exceptionHandler; // �쳣��������ַ
        static LONG NTAPI ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo); // �쳣������
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