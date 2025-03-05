#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "../Text.h"
#include "../Byteset.h"
namespace WinGui {
    typedef class winGui* PwinGui; // 前向声明
    /*
    * 窗口回调函数类型定义
    * PWinGui: 指向 WinGui 类的指针
    * HDC: 设备上下文句柄
    * BOOL: 窗口状态,true:窗口初始化,false:窗口销毁
    */
    typedef void (*WinGuiCallback)(PwinGui, HWND, BOOL);
    //窗口样式结构体
    struct WINSTYLE {
        bool hasBorder;             // 窗口具有细线边框, 默认为 true
        bool hasCaption;            // 窗口具有标题栏, 默认为 true
        bool isChild;               // 窗口是子窗口, 默认为 false
        bool isVisible;             // 窗口最初可见, 默认为 true
        bool hasMinimizeBox;        // 窗口具有最小化按钮, 默认为 true
        bool hasMaximizeBox;        // 窗口具有最大化按钮, 默认为 true
        bool hasSysMenu;            // 窗口的标题栏上有一个窗口菜单, 默认为 true
        bool hasThickFrame;         // 窗口具有大小调整边框, 默认为 false
        bool hasHScroll;            // 窗口具有水平滚动条, 默认为 false
        bool hasVScroll;            // 窗口具有垂直滚动条, 默认为 false
        bool isDisabled;            // 窗口最初处于禁用状态, 默认为 false
        bool hasDlgFrame;           // 窗口边框样式通常与对话框相同, 默认为 false
        bool isGroup;               // 窗口是一组控件中的第一个控件, 默认为 false
        bool isMaximized;           // 窗口最初是最大化的, 默认为 false
        bool isMinimized;           // 窗口最初是最小化的, 默认为 false
        bool isPopup;               // 窗口是弹出窗口, 默认为 false
        bool isClipChildren;        // 在父窗口内进行绘图时，不包括子窗口, 默认为 false
        bool isClipSiblings;        // 相对于彼此剪裁子窗口, 默认为 false
        bool isDialog;              // 窗口是对话框, 默认为 false
        bool hasTabStop;            // 窗口是一个控件，当用户按下 Tab 键时，该控件可以接收焦点, 默认为 false
        bool isOverlapped;          // 窗口是重叠的窗口, 默认为 true
        bool isTiled;               // 窗口是平铺窗口, 默认为 true

        // 构造函数，提供默认值
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
    // 字体样式结构体
    struct FONTSTYLE {
        int size = 16;                                                             // 字体大小, 默认 16像素
        int chatWidth = NULL;                                              //字符宽度
        int  rotation = NULL;                                                 //旋转角度
        int italicAngle = NULL;                                              //倾斜角度
        bool strikeOut = FALSE;                                            // 是否删除线
        bool underline = FALSE;                                            // 是否下划线
        bool italic = FALSE;                                                   // 是否斜体
        bool bold = FALSE;                                                    // 是否加粗
        std::wstring fontName = TEXT("Arial");                     // 字体名称, 默认 Arial
        std::wstring fontPath;                                                // TTF 格式字体文件路径（可选，默认值为空）
        bool verticalCenter = TRUE;                                      //垂直居中
    };
    // 组件操作事件结构体
    typedef struct COMPONENTEVENT {
        /**
         * 鼠标左键点击事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         */
        typedef void (*OnClick)(HWND hwnd);
        OnClick onClick = nullptr;
        /**
         * 鼠标悬停事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         * @param x: 当前鼠标的 x 坐标，表示鼠标在窗口客户区域内的位置
         * @param y: 当前鼠标的 y 坐标，表示鼠标在窗口客户区域内的位置
         */
        typedef void (*OnHover)(HWND hwnd, int x, int y);
        OnHover onHover = nullptr;
        /**
         * 鼠标离开事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         */
        typedef void (*OnMouseLeave)(HWND hwnd);
        OnMouseLeave onMouseLeave = nullptr;
        /**
         * 鼠标右键点击事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         * @param x: 当前鼠标的 x 坐标，表示鼠标在窗口客户区域内的位置
         * @param y: 当前鼠标的 y 坐标，表示鼠标在窗口客户区域内的位置
         */
        typedef void (*OnRightClick)(HWND hwnd, int x, int y);
        OnRightClick onRightClick = nullptr;
        /**
         * 鼠标双击事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         * @param x: 当前鼠标的 x 坐标，表示鼠标在窗口客户区域内的位置
         * @param y: 当前鼠标的 y 坐标，表示鼠标在窗口客户区域内的位置
         */
        typedef void (*OnDoubleClick)(HWND hwnd, int x, int y);
        OnDoubleClick onDoubleClick = nullptr;
        /**
         * 鼠标移动事件回调函数
         * @param x: 当前鼠标的 x 坐标，表示鼠标在窗口客户区域内的位置
         * @param y: 当前鼠标的 y 坐标，表示鼠标在窗口客户区域内的位置
         */
        typedef void (*OnMouseMove)(HWND hwnd, int x, int y);
        OnMouseMove onMouseMove = nullptr;
        /**
         * 键盘按下事件回调函数
         * @param key: 按下的虚拟键值
         */
        typedef void (*OnKeyDown)(HWND hwnd, unsigned char key);
        OnKeyDown onKeyDown = nullptr;
        /**
         * 键盘抬起事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         * @param key: 按下的虚拟键值
         */
        typedef void (*OnKeyUp)(HWND hwnd, unsigned char key);
        OnKeyUp onKeyUp = nullptr;
        /**
         * 焦点获得事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         */
        typedef void (*OnFocusGain)(HWND hwnd);
        OnFocusGain onFocusGain = nullptr;
        /**
         * 焦点失去事件回调函数
         * @param hwnd: 窗口句柄，表示触发事件的窗口
         */
        typedef void (*OnFocusLose)(HWND hwnd);
        OnFocusLose onFocusLose = nullptr;
    } COMPONENTEVENT, * PCOMPONENTEVENT;
    // 窗口信息结构体
    struct WININFO {
        int width = NULL;                    // 窗口宽度（必填）
        int height = NULL;                   // 窗口高度（必填）
        int left = NULL;                     // 窗口左边距（必填）
        int top = NULL;                      // 窗口顶部（必填）
        std::wstring title = L"";           // 窗口标题（必填）
        std::wstring className = L"";       // 窗口类名（必填）
        std::wstring* text = nullptr;            // 窗口文本内容（可选，如果组件为文本框，则需要传入指针）
        bool* isChecked = nullptr;              //选择框选择状态
        FONTSTYLE fontStyle = FONTSTYLE();             // 字体样式（可空）
        HWND hwnd = NULL;                    // 窗口句柄 (自动生成)
        HWND parentHwnd = NULL;              // 父窗口句柄 (可选，默认值 NULL)
        int hMenu = -1;                   // 控件ID (可选，默认值 -1)
        WINSTYLE style = WINSTYLE();               // 窗口样式 (可选，默认值)
        WinGuiCallback callback = nullptr;       // 窗口回调函数 (可选，默认值为空)
        HINSTANCE hInstance = NULL;          // 窗口实例句柄（可选，默认值为 GetModuleHandle(NULL)）
        COLORREF color = RGB(0, 0, 0);                  //字体颜色, 默认黑色
        COLORREF bgColor = RGB(240, 240, 240);             // 窗口背景颜色（可选，默认值）
        std::wstring imagePath = L"";        // 图片路径（可选，默认值为空）
        std::wstring iconPath = L"";        // 窗口图标路径（可选，默认值为空）
        DWORD iconId = NULL;                      // 图标ID（可选，默认值0）,可用于C++资源文件中的id来加载图标
        HFONT hFont = nullptr;                  // 字体句柄（可选，默认值 NULL）
        bool isMainWindow = false;            // 是否为主窗口（可选，默认值为 false）
        COMPONENTEVENT event = COMPONENTEVENT();         // 组件操作事件结构体（可选，默认值为空）
        std::vector<std::wstring> listData = {};                    // 列表框数据（可选，默认值为空）
        WNDCLASS wc = {};                   //窗口注册
        WININFO() = default;
        WININFO(
            const std::wstring& className,// 窗口类名
            const std::wstring& title,// 窗口标题
            int w, int h, int l, int topPos, // 宽度、高度、左边、顶边
            COLORREF bg = RGB(240, 240, 240), // 默认背景颜色为浅灰色
            const std::wstring& icon = L"", // 默认图标路径为空
            const std::wstring& font = L"", // 默认字体路径为空
            const std::wstring& fontName = L"", // 默认字体名称为空
            bool mainWindow = false,// 默认不是主窗口
            WinGuiCallback callback = nullptr)
            : className(className), title(title), width(w), height(h),
            left(l), top(topPos), hInstance(GetModuleHandle(NULL)), // 确保使用有效的实例句柄
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
    // 窗口类
    class winGui {
    public:
        /*
        * 获取窗口结构
        * hwnd: 窗口控件ID
        * 返回值：窗口结构体
        */
        WININFO getWinInfo(unsigned int hmenu);
        /*
        * 添加窗口结构
        * info: 窗口信息结构体
        * 返回值：成功返回 true，失败返回 false
        */
        bool addWinInfo(WININFO& info);
        /*
        * 设置字体
        * info  窗口信息结构体
        */
        void setFont(WININFO& info);
    public:
        winGui();
        ~winGui();
        /*
        * 获取窗口结构
        * hwnd: 窗口句柄
        * 返回值：窗口结构体
        */
        WININFO getWinInfo(HWND hwnd);
        /*
        * 修改窗口结构
        * hwnd: 窗口句柄
        * info: 窗口信息结构体
        * 返回值：成功返回 true，失败返回 false
        */
        bool setWinInfo(HWND hwnd, WININFO& info);
        /*
        * 设置主窗口句柄
        * hwnd: 主窗口句柄
        */
        void setMainHwnd(HWND hwnd);
        /*
        * 创建窗口
        * info: 窗口信息结构体
        * 返回值：成功返回 true，失败返回 false
        */
        bool win_create(WININFO& info);
        /*
        * 销毁窗口
        * hwnd: 窗口句柄
        * 返回值：成功返回 true，失败返回 false
        */
        bool win_destroy(HWND hwnd);
        /*
        * 显示窗口
        * hwnd: 窗口句柄
        * isShow: 是否显示窗口
        * 返回值：成功返回 true，失败返回 false
        */
        bool win_show(HWND hwnd, bool isShow);
        /*
        * 设置窗口样式
        * hwnd: 窗口句柄
        * style: 窗口样式结构体
        * 返回值：成功返回 true，失败返回 false
        */
        bool win_style(HWND hwnd, WINSTYLE& style);
        // 运行主消息循环
        void Main();
        /*
        * 添加标签
        * hwnd: 父窗口句柄
        * info: 标签信息结构体
        * align: 标签对齐方式，默认左对齐,0为左对齐，1为居中，2为右对齐
        * 返回值：创建成功返回true,失败返回false
        */
        bool Label(HWND hwnd, WININFO& info, int align = NULL);
        /*
        * 置标签文本
        * hwnd: 标签句柄
        * text: 标签文本
        * size_px: 字体大小，默认16像素
        * 返回值：成功返回 true，失败返回 false
        */
        bool SetLabelText(HWND hwnd, const std::wstring& text, int size_px = 16);
        /*
        * 添加按钮
        * hwnd: 父窗口句柄
        * info: 按钮信息结构体
        * 返回值：创建成功返回true,失败返回false
        */
        bool Button(HWND hwnd, WININFO& info);
        /*
        * 置按钮文本
        * hwnd: 按钮句柄
        * text: 按钮文本
        * size_px: 字体大小，默认16像素
        * 返回值：成功返回 true，失败返回 false
        */
        bool SetButtonText(HWND hwnd, const std::wstring& text, int size_px = 16);
        /*
        * 添加文本框(最终的文本会在组件失去焦点后返回)
        * hwnd: 父窗口句柄
        * info: 输入框信息结构体
        * value: 输入框数据
        * size_px: 字体大小，默认16像素
        * 返回值：创建成功返回true,失败返回false
        */
        bool InputBox(HWND hwnd, WININFO& info, std::wstring* value = nullptr, int size_px = 16);
        /*
        * 置文本框内容
        * hwnd: 文本框句柄
        * value: 文本内容
        * size_px: 字体大小，默认16像素
        * 返回值：成功返回 true，失败返回 false
        */
        bool SetInputBoxText(HWND hwnd, const std::wstring& value, int size_px = 16);
        /*
        * 添加图片框
        * hwnd: 父窗口句柄
        * info: 图片框信息结构体
        * 返回值：创建成功返回true,失败返回false
        */
        bool PictureBox(HWND hwnd, WININFO& info);
        /*
        * 置图片框数据
        * hwnd: 图片框句柄
        * path: 新图片路径
        * 返回值: 成功返回true，失败返回 false
        */
        bool SetPictureBoxData(HWND hwnd, const std::wstring& path);
        /*
        * 添加选择框
        * hwnd: 父窗口句柄
        * info: 选择框信息结构体
        * isChecked: 选择框选择状态
        * 返回值：创建成功返回true,失败返回false
        */
        bool CheckBox(HWND hwnd, WININFO& info, bool* isChecked = nullptr);
        /*
        * 置选择框选择状态
        * hwnd: 选择框句柄
        * isChecked: 选择状态
        * 返回值：成功返回 true，失败返回 false
        */
        bool SetCheckBoxText(HWND hwnd, bool isChecked);
    private:
        std::vector<WININFO> winList; // 窗口列表
        HWND mainHwnd = NULL;                 // 主窗口句柄
        // 窗口消息处理函数
        static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
};