#pragma once
#include <functional>
//窗口事件
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
    * 鼠标:操作事件
    * @param int state 状态
        @tparam state状态值：WinGuiPlus::window::STATUS::MOUSE_ENTER: 鼠标进入
        @tparam state状态值：WinGuiPlus::window::STATUS::MOUSE_MOVE: 正常移动
        @tparam state状态值：WinGuiPlus::window::STATUS::MOUSE_LEFT_DOWN_MOVE: 按住左键移动
        @tparam state状态值：WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOWN_MOVE: 按住右键移动
        @tparam state状态值：WinGuiPlus::window::STATUS::MOUSE_LEAVE 鼠标离开
    * @param int x 鼠标坐标x
    * @param int y 鼠标坐标y
    */
    MouseMove mouse;
    /*
    * 鼠标:左键
    * @param int state 状态,0: 按下, 1: 放开
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_LEFT_DOWN: 左键按下
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_LEFT_UP: 左键松开
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_LEFT_DOUBLE_CLICK: 左键双击
    * @param int x 鼠标坐标x
    * @param int y 鼠标坐标y
    */
    MouseLeft mouseLeft;
    /*
    * 鼠标:右键
    * @param int state 状态,0: 按下, 1: 放开
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOWN: 右键按下
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_RIGHT_UP: 右键松开
        @tparam  state状态值：WinGuiPlus::window::STATUS::MOUSE_RIGHT_DOUBLE_CLICK: 右键双击
    * @param int x 鼠标坐标x
    * @param int y 鼠标坐标y
    */
    MouseRight mouseRight;
    /*
    * 键盘:操作
    * @param int state 状态,0: 按下, 1: 放开
        @tparam  state状态值：WinGuiPlus::window::STATUS::KEY_DOWN: 按下键
        @tparam  state状态值：WinGuiPlus::window::STATUS::KEY_UP: 松开键
    * @param int key 按下的键
    */
    Key key;
    /*
    * 窗口:大小变化
    * @param int width 宽度
    * @param int height 高度
    */
    WinSize winSize;
    /*
    * 窗口:位置变化
    * @param int x 左上角坐标x
    * @param int y 左上角坐标y
    */
    WinMove winMove;
    /*
    * 鼠标:滚轮滚动
    * @param int direction 方向
        @tparam  direction方向值：1 表示向上滚动，-1 表示向下滚动
    * @param int size 滚动大小
    * @param int x 鼠标坐标x
    * @param int y 鼠标坐标y
    */
    MouseWheel mouseWheel;
    /*
    * 窗口:失去焦点
    */
    KillFocus killFocus;
    /*
    * 窗口:设置光标
    */
    SetCursor setCursor;
}*PWINGUIPLUS_WINEVENTPROC;
//内部结构体
typedef struct WINGUIPLUS_TEMPLATE {
    std::string token = "";
    HWND hwnd = NULL;
    HINSTANCE hInstance = NULL;
    std::function<void(HWND hwnd, HINSTANCE hInstance, int status)> callBack = nullptr;
    WINGUIPLUS_WINEVENTPROC event{};
}*PWINGUIPLUS_TEMPLATE;
//对齐方式
enum ALIGN {
    TOP_LEFT, TOP_CENTER, TOP_RIGHT,//顶部:左边,中间,右边
    CENTER_LEFT, CENTER, CENTER_RIGHT,//中间:左边,中间,右边
    BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT//底部:左边,中间,右边
};
//组件标志类型
typedef enum COMPONENT_TYPE {
    LABEL, EDIT, PICTURE, CHECKBOX
} *PCOMPONENT_TYPE;
//窗口状态
enum WINGUIPLUS_STATUS {
    CREATE,//窗口创建
    DESTROY,//窗口销毁
    MOUSE_ENTER,//鼠标进入
    MOUSE_MOVE,//鼠标移动
    MOUSE_LEFT_DOWN_MOVE,//鼠标左键按下移动
    MOUSE_RIGHT_DOWN_MOVE,//鼠标右键按下移动
    MOUSE_LEAVE,//鼠标离开
    MOUSE_LEFT_DOWN,//鼠标左键按下
    MOUSE_LEFT_UP,//鼠标左键松开
    MOUSE_LEFT_DOUBLE_CLICK,//鼠标左键双击
    MOUSE_RIGHT_DOWN,//鼠标右键按下
    MOUSE_RIGHT_UP,//鼠标右键松开
    MOUSE_RIGHT_DOUBLE_CLICK,//鼠标右键双击
    KEY_DOWN,//键盘按下
    KEY_UP//键盘松开
};
//矩形结构体
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