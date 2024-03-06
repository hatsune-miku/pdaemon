module;

#include <Windows.h>

/*

Adding a InputBox() to any c++ program - an article for Code Project
by Michael Haephrati, Secured Globe, Inc. www.securedglobe.net
haephrati@gmail.com

June 2019
?019 Michael Haephrati and Secured Globe, Inc. 1501 Broadway Ave. STE 1200, New York 10036, NY
*/

#define ASPECT_RATIO_X	2
#define ASPECT_RATIO_Y	2
#define TOP_EDGE		10 * ASPECT_RATIO_Y
#define INPUTBOX_WIDTH	500 * ASPECT_RATIO_X
#define INPUTBOX_HEIGHT 150 * ASPECT_RATIO_Y
#define TEXTEDIT_HEIGHT	30 * ASPECT_RATIO_Y
#define BUTTON_HEIGHT	25 * ASPECT_RATIO_Y
#define BUTTON_WIDTH	120 * ASPECT_RATIO_X
#define FONT_HEIGHT		20 * ASPECT_RATIO_Y

#define CLASSNAME					L"SG_Inputbox"
#define PUSH_BUTTON					L"Button"
#define FONT_NAME					TEXT("Times")
#define TEXTEDIT_CLASS				L"edit"
#define SetFontToControl(n)			SendMessage((n), WM_SETFONT, (WPARAM)m_hFont, 0);

export module ui_utils;

HFONT m_hFont = NULL;
HWND  m_hWndInputBox = NULL;
HWND  m_hWndParent = NULL;
HWND  m_hWndEdit = NULL;
HWND  m_hWndOK = NULL;
HWND  m_hWndCancel = NULL;
HWND  m_hWndPrompt = NULL;
wchar_t m_String[320];
static HBRUSH hbrBkgnd = NULL;

export class SG_InputBox
{
	static HFONT m_hFont;
	static HWND  m_hWndInputBox;
	static HWND  m_hWndParent;
	static HWND  m_hWndEdit;
	static HWND  m_hWndOK;
	static HWND  m_hWndCancel;
	static HWND  m_hWndPrompt;
	static wchar_t m_String[320];
	static HBRUSH hbrBkgnd;


	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static LPWSTR GetString(LPCTSTR szCaption, LPCTSTR szPrompt, LPCTSTR szDefaultText = L"");

};


/*

Adding a InputBox() to any c++ program - an article for Code Project
by Michael Haephrati, Secured Globe, Inc. www.securedglobe.net
haephrati@gmail.com

June 2019
?019 Michael Haephrati and Secured Globe, Inc. 1501 Broadway Ave. STE 1200, New York 10036, NY
*/

HFONT SG_InputBox::m_hFont = NULL;
HWND  SG_InputBox::m_hWndInputBox = NULL;
HWND  SG_InputBox::m_hWndParent = NULL;
HWND  SG_InputBox::m_hWndEdit = NULL;
HWND  SG_InputBox::m_hWndOK = NULL;
HWND  SG_InputBox::m_hWndCancel = NULL;
HWND  SG_InputBox::m_hWndPrompt = NULL;
wchar_t SG_InputBox::m_String[320];
HBRUSH SG_InputBox::hbrBkgnd = NULL;

#define SOFT_BLUE RGB(206,214,240)
LRESULT CALLBACK SG_InputBox::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LOGFONT lfont;
	HINSTANCE m_hInst = NULL;
	switch (message)
	{
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(SOFT_BLUE);
		}
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, SOFT_BLUE);

		return (INT_PTR)hbrBkgnd;
	}
	break;
	case WM_CREATE:
		// Define out font
		memset(&lfont, 0, sizeof(lfont));
		lstrcpy(lfont.lfFaceName, FONT_NAME);
		lfont.lfHeight = FONT_HEIGHT;
		lfont.lfWeight = FW_NORMAL;//FW_BOLD;
		lfont.lfItalic = FALSE;
		lfont.lfCharSet = DEFAULT_CHARSET;
		lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lfont.lfQuality = DEFAULT_QUALITY;
		lfont.lfPitchAndFamily = DEFAULT_PITCH;
		m_hFont = CreateFontIndirect(&lfont);
		// 
		m_hInst = GetModuleHandle(NULL);

		// The TextEdit Control - For the text to be input
		m_hWndEdit = CreateWindowEx(WS_EX_STATICEDGE,
			TEXTEDIT_CLASS, L"",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
			5, TOP_EDGE + BUTTON_HEIGHT * 2 + 30, INPUTBOX_WIDTH - 30, TEXTEDIT_HEIGHT,
			hWnd,
			NULL,
			m_hInst,
			NULL);

		if (m_hWndEdit == NULL)
		{
			return NULL;
		}

		SetFontToControl(m_hWndEdit);

		// The Confirm button
		m_hWndOK = CreateWindowEx(WS_EX_STATICEDGE,
			PUSH_BUTTON, L"Confirm",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			INPUTBOX_WIDTH - BUTTON_WIDTH - 30, TOP_EDGE, BUTTON_WIDTH, BUTTON_HEIGHT,
			hWnd,
			NULL,
			m_hInst,
			NULL);
		if (m_hWndOK == NULL)
		{
			return NULL;
		}
		// setting font
		SetFontToControl(m_hWndOK);

		// The Cancel button
		m_hWndCancel = CreateWindowEx(WS_EX_STATICEDGE,
			PUSH_BUTTON, L"Cancel",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			INPUTBOX_WIDTH - BUTTON_WIDTH - 30, TOP_EDGE + BUTTON_HEIGHT + 15, BUTTON_WIDTH, BUTTON_HEIGHT,
			hWnd,
			NULL,
			m_hInst,
			NULL);

		if (m_hWndCancel == NULL)
		{
			return NULL;
		}

		// setting font
		SetFontToControl(m_hWndCancel);

		// The SG_InputBox Caption Static text
		m_hWndPrompt = CreateWindowEx(WS_EX_STATICEDGE,
			L"static", L"",
			WS_VISIBLE | WS_CHILD,
			5, TOP_EDGE, INPUTBOX_WIDTH - BUTTON_WIDTH - 50, BUTTON_HEIGHT * 2 + TOP_EDGE,
			hWnd,
			NULL,
			m_hInst,
			NULL);

		if (m_hWndPrompt == NULL)
		{
			return NULL;
		}

		// setting font
		SetFontToControl(m_hWndPrompt);
		SetFocus(m_hWndEdit);

		break;
	case WM_DESTROY:

		DeleteObject(m_hFont);


		EnableWindow(m_hWndParent, TRUE);
		SetForegroundWindow(m_hWndParent);
		DestroyWindow(hWnd);
		PostQuitMessage(0);

		break;
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{

		case BN_CLICKED:
			if ((HWND)lParam == m_hWndOK)
				PostMessage(m_hWndInputBox, WM_KEYDOWN, VK_RETURN, 0);
			if ((HWND)lParam == m_hWndCancel)
				PostMessage(m_hWndInputBox, WM_KEYDOWN, VK_ESCAPE, 0);
			break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LPWSTR SG_InputBox::GetString(LPCTSTR szCaption, LPCTSTR szPrompt, LPCTSTR szDefaultText)
{
	RECT r;
	HWND hWnd = GetDesktopWindow();
	GetWindowRect(hWnd, &r);

	HINSTANCE hInst = GetModuleHandle(NULL);

	WNDCLASSEX wcex;

	if (!GetClassInfoEx(hInst, CLASSNAME, &wcex))
	{
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;//LoadIcon(hInst, (LPCTSTR)IDI_MYINPUTBOX);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = CLASSNAME;
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
	}

	m_hWndParent = hWnd;

	m_hWndInputBox = CreateWindowEx(/*WS_EX_TOOLWINDOW*/WS_EX_DLGMODALFRAME,
		CLASSNAME,
		szCaption,
		WS_POPUPWINDOW | WS_CAPTION | WS_TABSTOP,
		(r.right - INPUTBOX_WIDTH) / 2, (r.bottom - INPUTBOX_HEIGHT) / 2,
		INPUTBOX_WIDTH, INPUTBOX_HEIGHT,
		m_hWndParent,
		NULL,
		NULL,
		NULL);
	if (m_hWndInputBox == NULL)
	{
		return NULL;
	}

	// Set text alingment
	SetWindowText(m_hWndPrompt, szPrompt);
	SetForegroundWindow(m_hWndInputBox);

	// Set default button
	SendMessage((HWND)m_hWndOK, BM_SETSTYLE,
		(WPARAM)LOWORD(BS_DEFPUSHBUTTON), MAKELPARAM(TRUE, 0));
	SendMessage((HWND)m_hWndCancel, BM_SETSTYLE,
		(WPARAM)LOWORD(BS_PUSHBUTTON), MAKELPARAM(TRUE, 0));

	// Set default text
	SendMessage(m_hWndEdit, EM_SETSEL, 0, -1);
	SendMessage(m_hWndEdit, EM_REPLACESEL, 0, (LPARAM)szDefaultText);
	SendMessage(m_hWndEdit, EM_SETSEL, 0, -1);
	SetFocus(m_hWndEdit);



	EnableWindow(m_hWndParent, FALSE);

	ShowWindow(m_hWndInputBox, SW_SHOW);
	UpdateWindow(m_hWndInputBox);

	BOOL ret = 0;

	MSG msg;

	HWND hWndFocused;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE)
			{
				SendMessage(m_hWndInputBox, WM_DESTROY, 0, 0);
				ret = 0;
			}
			if (msg.wParam == VK_RETURN)
			{
				int nCount = GetWindowTextLength(m_hWndEdit);
				nCount++;
				GetWindowText(m_hWndEdit, m_String, nCount);
				SendMessage(m_hWndInputBox, WM_DESTROY, 0, 0);
				ret = 1;
			}
			if (msg.wParam == VK_TAB)
			{
				hWndFocused = GetFocus();
				if (hWndFocused == m_hWndEdit) SetFocus(m_hWndOK);
				if (hWndFocused == m_hWndOK) SetFocus(m_hWndCancel);
				if (hWndFocused == m_hWndCancel) SetFocus(m_hWndEdit);
			}

		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return m_String;
}

