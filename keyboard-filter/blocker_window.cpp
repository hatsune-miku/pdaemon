#include <windows.h>

class Window {
private:
    HWND hwnd; // 主窗口句柄
    HWND hEdit; // 输入框句柄
    HWND hButton; // 按钮句柄
    HWND hLabel; // 标签句柄
    HINSTANCE hInstance; // 应用程序实例句柄

    // 窗口过程回调函数的静态包装器
    static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* window = nullptr;

        if (msg == WM_CREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }
        else {
            window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (window) {
            return window->WndProc(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    // 实际的窗口过程实例方法
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN && reinterpret_cast<HWND>(lParam) == hEdit) {
                // 输入框回车事件
                SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(1, BN_CLICKED), reinterpret_cast<LPARAM>(hButton));
            }
            break;
        case WM_COMMAND:
            if (wParam == 1) {
                setLabelText(TEXT("按钮被点击"));
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

public:
    Window(HINSTANCE hInst) : hInstance(hInst) {
        // 注册窗口类
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = Window::WndProcStatic;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = TEXT("WindowClass");
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        if (!RegisterClassEx(&wc)) {
            return;
        }

        // 创建窗口
        hwnd = CreateWindowEx(
            WS_EX_TOPMOST, TEXT("WindowClass"), TEXT("My Window"),
            WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            NULL, NULL, hInstance, this
        );

        if (!hwnd) {
            return;
        }

        // 创建输入框
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_PASSWORD,
            50, 50, 200, 20, hwnd, NULL, hInstance, NULL
        );

        // 创建按钮
        hButton = CreateWindow(
            TEXT("BUTTON"), TEXT("点击我"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            50, 80, 100, 24, hwnd, (HMENU)1, hInstance, NULL
        );

        // 创建标签
        hLabel = CreateWindow(
            TEXT("STATIC"), TEXT("准备就绪"), WS_VISIBLE | WS_CHILD,
            50, 110, 300, 20, hwnd, NULL, hInstance, NULL
        );

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
    }

    void setLabelText(const TCHAR* text) {
        SetWindowText(hLabel, text);
    }

    // 消息循环
    void MessageLoop() {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Window win(hInstance);
    win.MessageLoop();
    return 0;
}
