#include <windows.h>

class Window {
private:
    HWND hwnd; // �����ھ��
    HWND hEdit; // �������
    HWND hButton; // ��ť���
    HWND hLabel; // ��ǩ���
    HINSTANCE hInstance; // Ӧ�ó���ʵ�����

    // ���ڹ��̻ص������ľ�̬��װ��
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

    // ʵ�ʵĴ��ڹ���ʵ������
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN && reinterpret_cast<HWND>(lParam) == hEdit) {
                // �����س��¼�
                SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(1, BN_CLICKED), reinterpret_cast<LPARAM>(hButton));
            }
            break;
        case WM_COMMAND:
            if (wParam == 1) {
                setLabelText(TEXT("��ť�����"));
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
        // ע�ᴰ����
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

        // ��������
        hwnd = CreateWindowEx(
            WS_EX_TOPMOST, TEXT("WindowClass"), TEXT("My Window"),
            WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
            NULL, NULL, hInstance, this
        );

        if (!hwnd) {
            return;
        }

        // ���������
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_PASSWORD,
            50, 50, 200, 20, hwnd, NULL, hInstance, NULL
        );

        // ������ť
        hButton = CreateWindow(
            TEXT("BUTTON"), TEXT("�����"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            50, 80, 100, 24, hwnd, (HMENU)1, hInstance, NULL
        );

        // ������ǩ
        hLabel = CreateWindow(
            TEXT("STATIC"), TEXT("׼������"), WS_VISIBLE | WS_CHILD,
            50, 110, 300, 20, hwnd, NULL, hInstance, NULL
        );

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
    }

    void setLabelText(const TCHAR* text) {
        SetWindowText(hLabel, text);
    }

    // ��Ϣѭ��
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
