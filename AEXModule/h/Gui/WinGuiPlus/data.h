#pragma once
#include <functional>
//�����¼�
typedef struct WINGUIPLUS_WINEVENTPROC {
private:
    typedef std::function<void(int state, int x, int y)> MouseMove;
    typedef std::function<void(int state, int x, int y)> MouseLeft;
    typedef std::function<void(int state, int x, int y)> MouseRight;
    typedef std::function<void(int state, WPARAM& key, LPARAM& lParam)> Key;
    typedef std::function<void(int width, int height)> WinSize;
    typedef std::function<void(int x, int y)> WinMove;
    typedef std::function<void(int direction, int size, int x, int y)> MouseWheel;
    typedef std::function<void()> KillFocus;
    typedef std::function<void()> SetCursor;

public:
    /*
    * ���:�����¼�
    * @param int state ״̬
        @tparam state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_ENTER: ������
        @tparam state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_MOVE: �����ƶ�
        @tparam state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_LEFT_DOWN_MOVE: ��ס����ƶ�
        @tparam state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOWN_MOVE: ��ס�Ҽ��ƶ�
        @tparam state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_LEAVE ����뿪
    * @param int x �������x
    * @param int y �������y
    */
    MouseMove mouse;
    /*
    * ���:���
    * @param int state ״̬,0: ����, 1: �ſ�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_LEFT_DOWN: �������
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_LEFT_UP: ����ɿ�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_LEFT_DOUBLE_CLICK: ���˫��
    * @param int x �������x
    * @param int y �������y
    */
    MouseLeft mouseLeft;
    /*
    * ���:�Ҽ�
    * @param int state ״̬,0: ����, 1: �ſ�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOWN: �Ҽ�����
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_RIGHT_UP: �Ҽ��ɿ�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOUBLE_CLICK: �Ҽ�˫��
    * @param int x �������x
    * @param int y �������y
    */
    MouseRight mouseRight;
    /*
    * ����:����
    * @param int state ״̬,0: ����, 1: �ſ�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::KEY_DOWN: ���¼�
        @tparam  state״ֵ̬��WinGuiPlus::window::STATUS::KEY_UP: �ɿ���
    * @param int key ���µļ�
    */
    Key key;
    /*
    * ����:��С�仯
    * @param int width ���
    * @param int height �߶�
    */
    WinSize winSize;
    /*
    * ����:λ�ñ仯
    * @param int x ���Ͻ�����x
    * @param int y ���Ͻ�����y
    */
    WinMove winMove;
    /*
    * ���:���ֹ���
    * @param int direction ����
        @tparam  direction����ֵ��1 ��ʾ���Ϲ�����-1 ��ʾ���¹���
    * @param int size ������С
    * @param int x �������x
    * @param int y �������y
    */
    MouseWheel mouseWheel;
    /*
    * ����:ʧȥ����
    */
    KillFocus killFocus;
    /*
    * ����:���ù��
    */
    SetCursor setCursor;
}*PWINGUIPLUS_WINEVENTPROC;
//�ڲ��ṹ��
typedef struct WINGUIPLUS_TEMPLATE {
    std::string token = "";
    HWND hwnd = NULL;
    HINSTANCE hInstance = NULL;
    std::function<void(HWND hwnd, HINSTANCE hInstance, int status)> callBack = nullptr;
    WINGUIPLUS_WINEVENTPROC event{};
}*PWINGUIPLUS_TEMPLATE;
//���뷽ʽ
enum ALIGN {
    TOP_LEFT, TOP_CENTER, TOP_RIGHT,//����:���,�м�,�ұ�
    CENTER_LEFT, CENTER, CENTER_RIGHT,//�м�:���,�м�,�ұ�
    BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT//�ײ�:���,�м�,�ұ�
};
//�����־����
typedef enum COMPONENT_TYPE {
    LABEL, EDIT, PICTURE, CHECKBOX
} *PCOMPONENT_TYPE;
//����״̬
enum WINGUIPLUS_STATUS {
    CREATE,//���ڴ���
    DESTROY,//��������
    MOUSE_ENTER,//������
    MOUSE_MOVE,//����ƶ�
    MOUSE_LEFT_DOWN_MOVE,//�����������ƶ�
    MOUSE_RIGHT_DOWN_MOVE,//����Ҽ������ƶ�
    MOUSE_LEAVE,//����뿪
    MOUSE_LEFT_DOWN,//����������
    MOUSE_LEFT_UP,//�������ɿ�
    MOUSE_LEFT_DOUBLE_CLICK,//������˫��
    MOUSE_RIGHT_DOWN,//����Ҽ�����
    MOUSE_RIGHT_UP,//����Ҽ��ɿ�
    MOUSE_RIGHT_DOUBLE_CLICK,//����Ҽ�˫��
    KEY_DOWN,//���̰���
    KEY_UP//�����ɿ�
};
//���νṹ��
typedef struct WINGUIPLUS_RECT {
    ALIGN align;
    ALIGN alignType;
    int top, left, width, height;
}*PWINGUIPLUS_RECT;

static void SetWinRectPositionAlign(WINGUIPLUS_RECT winInfo, int& x, int& y) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    switch (winInfo.align) {
    case TOP_LEFT: x = 0; y = 0; break;
    case TOP_CENTER: x = (screenWidth - winInfo.width) / 2; y = 0;  break;
    case TOP_RIGHT: x = screenWidth - winInfo.width; y = 0; break;
    case CENTER_LEFT: x = 0; y = (screenHeight - winInfo.height) / 2; break;
    case CENTER: x = (screenWidth - winInfo.width) / 2; y = (screenHeight - winInfo.height) / 2; break;
    case CENTER_RIGHT: x = screenWidth - winInfo.width; y = (screenHeight - winInfo.height) / 2; break;
    case BOTTOM_LEFT: x = 0; y = screenHeight - winInfo.height; break;
    case BOTTOM_CENTER: x = (screenWidth - winInfo.width) / 2; y = screenHeight - winInfo.height; break;
    case BOTTOM_RIGHT: x = screenWidth - winInfo.width; y = screenHeight - winInfo.height; break;
    default: x = CW_USEDEFAULT; y = CW_USEDEFAULT; break;
    }
}
static void SetWinRectPosition(WINGUIPLUS_RECT winInfo, int& x, int& y) {
    int x_(x), y_(y);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int gridWidth = screenWidth / 3;
    int gridHeight = screenHeight / 3;
    switch (winInfo.align) {
    case TOP_LEFT:x = 0; y = 0; break;
    case TOP_CENTER:  x = gridWidth; y = 0; break;
    case TOP_RIGHT: x = gridWidth * 2;  y = 0; break;
    case CENTER_LEFT: x = 0; y = gridHeight; break;
    case CENTER: x = gridWidth; y = gridHeight; break;
    case CENTER_RIGHT: x = gridWidth * 2; y = gridHeight; break;
    case BOTTOM_LEFT: x = 0; y = gridHeight * 2; break;
    case BOTTOM_CENTER: x = gridWidth; y = gridHeight * 2; break;
    case BOTTOM_RIGHT: x = gridWidth * 2; y = gridHeight * 2; break;
    default: x = CW_USEDEFAULT; y = CW_USEDEFAULT; break;
    }
    switch (winInfo.alignType) {
    case TOP_LEFT: x += 0; y += 0; break;
    case TOP_CENTER: x += (gridWidth - winInfo.width) / 2; y += 0; break;
    case TOP_RIGHT: x += gridWidth - winInfo.width; y += 0; break;
    case CENTER_LEFT: x += 0;  y += (gridHeight - winInfo.height) / 2; break;
    case CENTER: x += (gridWidth - winInfo.width) / 2; y += (gridHeight - winInfo.height) / 2; break;
    case CENTER_RIGHT: x += gridWidth - winInfo.width; y += (gridHeight - winInfo.height) / 2; break;
    case BOTTOM_LEFT: x += 0; y += gridHeight - winInfo.height; break;
    case BOTTOM_CENTER: x += (gridWidth - winInfo.width) / 2; y += gridHeight - winInfo.height; break;
    case BOTTOM_RIGHT: x += gridWidth - winInfo.width; y += gridHeight - winInfo.height; break;
    default: x += 0; y += 0; break;
    }
    if (winInfo.width > gridWidth || winInfo.height > gridHeight) SetWinRectPositionAlign(winInfo, x, y);
    if (winInfo.top != NULL) y = y_ + winInfo.top;
    if (winInfo.left != NULL) x = x_ + winInfo.left;
}