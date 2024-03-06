// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

#include <iostream>

BOOL should_block = FALSE;
HHOOK g_hHook = NULL;

const int VK0 = 0x30;
const int VK9 = 0x39;

// Allow only numbers (VK0-VK9, VK_NUMPAD0, VK_NUMPAD9) and enter
BOOL is_key_allowed(int vkCode) {
	return (vkCode >= VK0 && vkCode <= VK9)
		|| (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
		|| vkCode == VK_RETURN;
}

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
LRESULT CALLBACK proc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (should_block) {
		if (nCode == HC_ACTION) {
			KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
			if (!is_key_allowed(kbd->vkCode)) {
				std::cout << "Blocked key: " << kbd->vkCode << std::endl;
				return 1;
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void install_hook() {
	if (g_hHook) {
		return;
	}
	g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(NULL), 0);
	if (!g_hHook) {
		std::cout << "Failed to install hook" << std::endl;
		std::cout << "Error: " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Hook installed" << std::endl;
	}
}

extern "C" __declspec(dllexport) void uninstall_hook() {
	UnhookWindowsHookEx(g_hHook);
	g_hHook = NULL;
	std::cout << "Hook uninstalled" << std::endl;
}

extern "C" __declspec(dllexport) void start_hook() {
	should_block = TRUE;
	std::cout << "Hook started" << std::endl;
}

extern "C" __declspec(dllexport) void stop_hook() {
	should_block = FALSE;
	std::cout << "Hook stopped" << std::endl;
}
