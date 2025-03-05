#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "../Text.h"
#include "../Byteset.h"
namespace WinGui {
    typedef class winGui* PwinGui; // ǰ������
    /*
    * ���ڻص��������Ͷ���
    * PWinGui: ָ�� WinGui ���ָ��
    * HDC: �豸�����ľ��
    * BOOL: ����״̬,true:���ڳ�ʼ��,false:��������
    */
    typedef void (*WinGuiCallback)(PwinGui, HWND, BOOL);
    //������ʽ�ṹ��
    struct WINSTYLE {
        bool hasBorder;             // ���ھ���ϸ�߱߿�, Ĭ��Ϊ true
        bool hasCaption;            // ���ھ��б�����, Ĭ��Ϊ true
        bool isChild;               // �������Ӵ���, Ĭ��Ϊ false
        bool isVisible;             // ��������ɼ�, Ĭ��Ϊ true
        bool hasMinimizeBox;        // ���ھ�����С����ť, Ĭ��Ϊ true
        bool hasMaximizeBox;        // ���ھ�����󻯰�ť, Ĭ��Ϊ true
        bool hasSysMenu;            // ���ڵı���������һ�����ڲ˵�, Ĭ��Ϊ true
        bool hasThickFrame;         // ���ھ��д�С�����߿�, Ĭ��Ϊ false
        bool hasHScroll;            // ���ھ���ˮƽ������, Ĭ��Ϊ false
        bool hasVScroll;            // ���ھ��д�ֱ������, Ĭ��Ϊ false
        bool isDisabled;            // ����������ڽ���״̬, Ĭ��Ϊ false
        bool hasDlgFrame;           // ���ڱ߿���ʽͨ����Ի�����ͬ, Ĭ��Ϊ false
        bool isGroup;               // ������һ��ؼ��еĵ�һ���ؼ�, Ĭ��Ϊ false
        bool isMaximized;           // �����������󻯵�, Ĭ��Ϊ false
        bool isMinimized;           // �����������С����, Ĭ��Ϊ false
        bool isPopup;               // �����ǵ�������, Ĭ��Ϊ false
        bool isClipChildren;        // �ڸ������ڽ��л�ͼʱ���������Ӵ���, Ĭ��Ϊ false
        bool isClipSiblings;        // ����ڱ˴˼����Ӵ���, Ĭ��Ϊ false
        bool isDialog;              // �����ǶԻ���, Ĭ��Ϊ false
        bool hasTabStop;            // ������һ���ؼ������û����� Tab ��ʱ���ÿؼ����Խ��ս���, Ĭ��Ϊ false
        bool isOverlapped;          // �������ص��Ĵ���, Ĭ��Ϊ true
        bool isTiled;               // ������ƽ�̴���, Ĭ��Ϊ true

        // ���캯�����ṩĬ��ֵ
        WINSTYLE()
            : hasBorder(true), hasCaption(true), isChild(false),
            isVisible(true), hasMinimizeBox(true), hasMaximizeBox(true),
            hasSysMenu(true), hasThickFrame(false), hasHScroll(false),
            hasVScroll(false), isDisabled(false), hasDlgFrame(false),
            isGroup(false), isMaximized(false), isMinimized(false),
            isPopup(false), isClipChildren(false), isClipSiblings(false),
            isDialog(false), hasTabStop(false), isOverlapped(true),
            isTiled(true) {
        }
    };
    // ������ʽ�ṹ��
    struct FONTSTYLE {
        int size = 16;                                                             // �����С, Ĭ�� 16����
        int chatWidth = NULL;                                              //�ַ����
        int  rotation = NULL;                                                 //��ת�Ƕ�
        int italicAngle = NULL;                                              //��б�Ƕ�
        bool strikeOut = FALSE;                                            // �Ƿ�ɾ����
        bool underline = FALSE;                                            // �Ƿ��»���
        bool italic = FALSE;                                                   // �Ƿ�б��
        bool bold = FALSE;                                                    // �Ƿ�Ӵ�
        std::wstring fontName = TEXT("Arial");                     // ��������, Ĭ�� Arial
        std::wstring fontPath;                                                // TTF ��ʽ�����ļ�·������ѡ��Ĭ��ֵΪ�գ�
        bool verticalCenter = TRUE;                                      //��ֱ����
    };
    // ��������¼��ṹ��
    typedef struct COMPONENTEVENT {
        /**
         * ����������¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         */
        typedef void (*OnClick)(HWND hwnd);
        OnClick onClick = nullptr;
        /**
         * �����ͣ�¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         * @param x: ��ǰ���� x ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         * @param y: ��ǰ���� y ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         */
        typedef void (*OnHover)(HWND hwnd, int x, int y);
        OnHover onHover = nullptr;
        /**
         * ����뿪�¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         */
        typedef void (*OnMouseLeave)(HWND hwnd);
        OnMouseLeave onMouseLeave = nullptr;
        /**
         * ����Ҽ�����¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         * @param x: ��ǰ���� x ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         * @param y: ��ǰ���� y ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         */
        typedef void (*OnRightClick)(HWND hwnd, int x, int y);
        OnRightClick onRightClick = nullptr;
        /**
         * ���˫���¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         * @param x: ��ǰ���� x ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         * @param y: ��ǰ���� y ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         */
        typedef void (*OnDoubleClick)(HWND hwnd, int x, int y);
        OnDoubleClick onDoubleClick = nullptr;
        /**
         * ����ƶ��¼��ص�����
         * @param x: ��ǰ���� x ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         * @param y: ��ǰ���� y ���꣬��ʾ����ڴ��ڿͻ������ڵ�λ��
         */
        typedef void (*OnMouseMove)(HWND hwnd, int x, int y);
        OnMouseMove onMouseMove = nullptr;
        /**
         * ���̰����¼��ص�����
         * @param key: ���µ������ֵ
         */
        typedef void (*OnKeyDown)(HWND hwnd, unsigned char key);
        OnKeyDown onKeyDown = nullptr;
        /**
         * ����̧���¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         * @param key: ���µ������ֵ
         */
        typedef void (*OnKeyUp)(HWND hwnd, unsigned char key);
        OnKeyUp onKeyUp = nullptr;
        /**
         * �������¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         */
        typedef void (*OnFocusGain)(HWND hwnd);
        OnFocusGain onFocusGain = nullptr;
        /**
         * ����ʧȥ�¼��ص�����
         * @param hwnd: ���ھ������ʾ�����¼��Ĵ���
         */
        typedef void (*OnFocusLose)(HWND hwnd);
        OnFocusLose onFocusLose = nullptr;
    } COMPONENTEVENT, * PCOMPONENTEVENT;
    // ������Ϣ�ṹ��
    struct WININFO {
        int width = NULL;                    // ���ڿ�ȣ����
        int height = NULL;                   // ���ڸ߶ȣ����
        int left = NULL;                     // ������߾ࣨ���
        int top = NULL;                      // ���ڶ��������
        std::wstring title = L"";           // ���ڱ��⣨���
        std::wstring className = L"";       // �������������
        std::wstring* text = nullptr;            // �����ı����ݣ���ѡ��������Ϊ�ı�������Ҫ����ָ�룩
        bool* isChecked = nullptr;              //ѡ���ѡ��״̬
        FONTSTYLE fontStyle = FONTSTYLE();             // ������ʽ���ɿգ�
        HWND hwnd = NULL;                    // ���ھ�� (�Զ�����)
        HWND parentHwnd = NULL;              // �����ھ�� (��ѡ��Ĭ��ֵ NULL)
        int hMenu = -1;                   // �ؼ�ID (��ѡ��Ĭ��ֵ -1)
        WINSTYLE style = WINSTYLE();               // ������ʽ (��ѡ��Ĭ��ֵ)
        WinGuiCallback callback = nullptr;       // ���ڻص����� (��ѡ��Ĭ��ֵΪ��)
        HINSTANCE hInstance = NULL;          // ����ʵ���������ѡ��Ĭ��ֵΪ GetModuleHandle(NULL)��
        COLORREF color = RGB(0, 0, 0);                  //������ɫ, Ĭ�Ϻ�ɫ
        COLORREF bgColor = RGB(240, 240, 240);             // ���ڱ�����ɫ����ѡ��Ĭ��ֵ��
        std::wstring imagePath = L"";        // ͼƬ·������ѡ��Ĭ��ֵΪ�գ�
        std::wstring iconPath = L"";        // ����ͼ��·������ѡ��Ĭ��ֵΪ�գ�
        DWORD iconId = NULL;                      // ͼ��ID����ѡ��Ĭ��ֵ0��,������C++��Դ�ļ��е�id������ͼ��
        HFONT hFont = nullptr;                  // ����������ѡ��Ĭ��ֵ NULL��
        bool isMainWindow = false;            // �Ƿ�Ϊ�����ڣ���ѡ��Ĭ��ֵΪ false��
        COMPONENTEVENT event = COMPONENTEVENT();         // ��������¼��ṹ�壨��ѡ��Ĭ��ֵΪ�գ�
        std::vector<std::wstring> listData = {};                    // �б�����ݣ���ѡ��Ĭ��ֵΪ�գ�
        WNDCLASS wc = {};                   //����ע��
        WININFO() = default;
        WININFO(
            const std::wstring& className,// ��������
            const std::wstring& title,// ���ڱ���
            int w, int h, int l, int topPos, // ��ȡ��߶ȡ���ߡ�����
            COLORREF bg = RGB(240, 240, 240), // Ĭ�ϱ�����ɫΪǳ��ɫ
            const std::wstring& icon = L"", // Ĭ��ͼ��·��Ϊ��
            const std::wstring& font = L"", // Ĭ������·��Ϊ��
            const std::wstring& fontName = L"", // Ĭ����������Ϊ��
            bool mainWindow = false,// Ĭ�ϲ���������
            WinGuiCallback callback = nullptr)
            : className(className), title(title), width(w), height(h),
            left(l), top(topPos), hInstance(GetModuleHandle(NULL)), // ȷ��ʹ����Ч��ʵ�����
            hwnd(NULL), color(RGB(0, 0, 0)), bgColor(bg), iconPath(icon),
            isMainWindow(mainWindow), callback(callback) {
            fontStyle.fontName = fontName;
            fontStyle.fontPath = font;
            hFont = nullptr;
            text = nullptr;
            hMenu = -1;
            isChecked = nullptr;
        }
    };
    // ������
    class winGui {
    public:
        /*
        * ��ȡ���ڽṹ
        * hwnd: ���ڿؼ�ID
        * ����ֵ�����ڽṹ��
        */
        WININFO getWinInfo(unsigned int hmenu);
        /*
        * ��Ӵ��ڽṹ
        * info: ������Ϣ�ṹ��
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool addWinInfo(WININFO& info);
        /*
        * ��������
        * info  ������Ϣ�ṹ��
        */
        void setFont(WININFO& info);
    public:
        winGui();
        ~winGui();
        /*
        * ��ȡ���ڽṹ
        * hwnd: ���ھ��
        * ����ֵ�����ڽṹ��
        */
        WININFO getWinInfo(HWND hwnd);
        /*
        * �޸Ĵ��ڽṹ
        * hwnd: ���ھ��
        * info: ������Ϣ�ṹ��
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool setWinInfo(HWND hwnd, WININFO& info);
        /*
        * ���������ھ��
        * hwnd: �����ھ��
        */
        void setMainHwnd(HWND hwnd);
        /*
        * ��������
        * info: ������Ϣ�ṹ��
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool win_create(WININFO& info);
        /*
        * ���ٴ���
        * hwnd: ���ھ��
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool win_destroy(HWND hwnd);
        /*
        * ��ʾ����
        * hwnd: ���ھ��
        * isShow: �Ƿ���ʾ����
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool win_show(HWND hwnd, bool isShow);
        /*
        * ���ô�����ʽ
        * hwnd: ���ھ��
        * style: ������ʽ�ṹ��
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool win_style(HWND hwnd, WINSTYLE& style);
        // ��������Ϣѭ��
        void Main();
        /*
        * ��ӱ�ǩ
        * hwnd: �����ھ��
        * info: ��ǩ��Ϣ�ṹ��
        * align: ��ǩ���뷽ʽ��Ĭ�������,0Ϊ����룬1Ϊ���У�2Ϊ�Ҷ���
        * ����ֵ�������ɹ�����true,ʧ�ܷ���false
        */
        bool Label(HWND hwnd, WININFO& info, int align = NULL);
        /*
        * �ñ�ǩ�ı�
        * hwnd: ��ǩ���
        * text: ��ǩ�ı�
        * size_px: �����С��Ĭ��16����
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool SetLabelText(HWND hwnd, const std::wstring& text, int size_px = 16);
        /*
        * ��Ӱ�ť
        * hwnd: �����ھ��
        * info: ��ť��Ϣ�ṹ��
        * ����ֵ�������ɹ�����true,ʧ�ܷ���false
        */
        bool Button(HWND hwnd, WININFO& info);
        /*
        * �ð�ť�ı�
        * hwnd: ��ť���
        * text: ��ť�ı�
        * size_px: �����С��Ĭ��16����
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool SetButtonText(HWND hwnd, const std::wstring& text, int size_px = 16);
        /*
        * ����ı���(���յ��ı��������ʧȥ����󷵻�)
        * hwnd: �����ھ��
        * info: �������Ϣ�ṹ��
        * value: ���������
        * size_px: �����С��Ĭ��16����
        * ����ֵ�������ɹ�����true,ʧ�ܷ���false
        */
        bool InputBox(HWND hwnd, WININFO& info, std::wstring* value = nullptr, int size_px = 16);
        /*
        * ���ı�������
        * hwnd: �ı�����
        * value: �ı�����
        * size_px: �����С��Ĭ��16����
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool SetInputBoxText(HWND hwnd, const std::wstring& value, int size_px = 16);
        /*
        * ���ͼƬ��
        * hwnd: �����ھ��
        * info: ͼƬ����Ϣ�ṹ��
        * ����ֵ�������ɹ�����true,ʧ�ܷ���false
        */
        bool PictureBox(HWND hwnd, WININFO& info);
        /*
        * ��ͼƬ������
        * hwnd: ͼƬ����
        * path: ��ͼƬ·��
        * ����ֵ: �ɹ�����true��ʧ�ܷ��� false
        */
        bool SetPictureBoxData(HWND hwnd, const std::wstring& path);
        /*
        * ���ѡ���
        * hwnd: �����ھ��
        * info: ѡ�����Ϣ�ṹ��
        * isChecked: ѡ���ѡ��״̬
        * ����ֵ�������ɹ�����true,ʧ�ܷ���false
        */
        bool CheckBox(HWND hwnd, WININFO& info, bool* isChecked = nullptr);
        /*
        * ��ѡ���ѡ��״̬
        * hwnd: ѡ�����
        * isChecked: ѡ��״̬
        * ����ֵ���ɹ����� true��ʧ�ܷ��� false
        */
        bool SetCheckBoxText(HWND hwnd, bool isChecked);
    private:
        std::vector<WININFO> winList; // �����б�
        HWND mainHwnd = NULL;                 // �����ھ��
        // ������Ϣ������
        static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
};