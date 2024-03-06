module;

#include <Windows.h>

export module daemon;

import <string>;
import <vector>;
import <iostream>;

import file_utils;
import byte_utils;
import ps_utils;
import taskschd_utils;

export struct DaemonContext {
	tstring process_path;
	tstring command_line;
};

export inline void msleep(DWORD dwMilliseconds) noexcept {
	Sleep(dwMilliseconds);
}

export bool mklnk(const tstring& pd_path, const tstring& target_path) noexcept {
	try {
		make_link(pd_path, target_path);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

export HRESULT mktask(
	const tstring& pd_path, 
	const tstring& target_path, 
	const tstring& additional_commandlines = TEXT("")
) noexcept {
	tstring real_target_path;
	try {
		real_target_path.assign(follow_lnk(target_path));
	}
	catch (const std::exception&) {
		return E_FAIL;
	}

	tstring target_file = basename(real_target_path);
	tstring task_name = std::format(TEXT("PDAEMON - {}"), target_file);
	tstring command_lines = std::format(TEXT("\"{}\""), real_target_path);

	if (!additional_commandlines.empty()) {
		command_lines += TEXT(" ") + additional_commandlines;
	}

	return create_login_task(
		task_name,
		TEXT("PDAEMON"),
		TEXT("Elevated startup & daemon"),
		pd_path,
		path_of(real_target_path),
		command_lines
	);
}

LRESULT CALLBACK window_proc_handler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept  {
	// Can exit?
	if (message == WM_QUERYENDSESSION) {
		return TRUE;
	}

	// Please exit
	if (message == WM_ENDSESSION) {
		exit(0);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

export void worker(HINSTANCE hInstance, const DaemonContext& context) noexcept {
	WNDCLASS wc = { 
		.lpfnWndProc = window_proc_handler,
		.hInstance = hInstance,
		.lpszClassName = TEXT("HiddenWindowClass")
	};
	RegisterClass(&wc);

	// Create hidden window to receive WM_QUERYENDSESSION
	HWND hwnd = CreateWindowEx(
		0, TEXT("HiddenWindowClass"), TEXT("HiddenWindow"), 0, 0, 0, 0, 0, HWND_MESSAGE, 
		nullptr, hInstance, nullptr
	);

	while (true) {
		auto pi = create_process(context.process_path, context.command_line);
		if (pi.hProcess == nullptr) {
			MessageBoxA(nullptr, "Failed to create process", "Error", MB_ICONERROR);
			break;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	};
}
