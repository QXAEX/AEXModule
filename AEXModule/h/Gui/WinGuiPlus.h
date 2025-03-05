#pragma once
#include "../Text.h"
#include <windows.h>
#include <string>
#include "WinGuiPlus/data.h"
#include "./WinGuiPlus/Label.h"
#include "./WinGuiPlus/Edit.h"
#include "./WinGuiPlus/Picture.h"
#include "./WinGuiPlus/Button.h"
#include "./WinGuiPlus/CheckBox.h"
namespace WinGuiPlus {
    // ��Ϣѭ��
    int WINAPI run();
    // ������
    class window {
    public:
        //����λ��
        typedef struct INFO {
            ALIGN align = CENTER;//����Ļ�ж�Ϊ�Ź���,�����ھŹ����ڵ�λ�ã�Ĭ�Ͼ���
            ALIGN alignType = CENTER;//�ڸ����ڵ�λ�ã�Ĭ�Ͼ���
            int top = NULL;//�������Ͻ�����x,������align�����align�ľŹ���������м���
            int left = NULL;//�������Ͻ�����y,������align�����align�ľŹ���������м���
            int width = 400;//���
            int height = 300;//�߶�
            bool disableResize = true;//���ô��ڴ�С�仯,Ĭ�Ͻ���
            bool disableMaximize = true;//���ô������
            bool disableMinimize = false;//���ô�����С��
            bool topMost = false;//�����ö�
            BYTE alpha = 255;//����͸����
            DWORD iconId = NULL;// ͼ��ID����ѡ��Ĭ��ֵ0��,������C++��Դ�ļ��е�id������ͼ��
            DWORD style = WS_OVERLAPPEDWINDOW;//������ʽ��Ĭ��,��ʾ���ڿ����϶�,����,���,��С��
            COLORREF backgroundColor = RGB(255, 255, 255);//������ɫ
            WINGUIPLUS_WINEVENTPROC event;//�¼��ص�����

        } *PINFO;
        /*
        * ���ڻص�����
        * @param hwnd ���ھ��
        * @param status ״̬��,��Ӧ WINGUIPLUS_STATUS
        * @return void
        */
        typedef std::function<void WINAPI (HWND hwnd, HINSTANCE hInstance, int status)> callBack;
    public:
        window();
        ~window();
        /*
        * ��������
        * @param parent �����ھ��
        * @param title ���ڱ���
        * @param className ��������
        * @param winInfo ������Ϣ, ��createWinInfo��������
        * @return �Ƿ񴴽��ɹ�
        */
        HWND create(HWND parent, LPCWSTR title, LPCWSTR className, INFO winInfo, callBack callBackFunc = nullptr);
        /*
        * ��ȡINFO�ṹ��
        */
        WINGUIPLUS_TEMPLATE getInfo(HWND hwnd);
        /*
        * ����INFO�ṹ��
        */
        bool setInfo(WINGUIPLUS_TEMPLATE winInfo);
    private:
        HINSTANCE hInstance = NULL;
        std::string token = "";
        DWORD setWindowStyle(INFO& winInfo);
        DWORD setWindowExStyle(INFO& winInfo);
    };
    // ��ǩ
    using namespace WinGuiPlusLabel;
    // �༭��
    using namespace WinGuiPlusEdit;
    // ͼƬ
    using namespace WinGuiPlusPicture;
    // ��ť
    using namespace WinGuiPlusButton;
    // ��ѡ��
    using namespace WinGuiPlusCheckBox;
}
