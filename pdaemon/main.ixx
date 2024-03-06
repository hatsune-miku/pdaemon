#include <Windows.h>

import <format>;
import <string>;

import daemon;
import byte_utils;
import ui_utils;

const tstring EXT_MKLNK = TEXT(".mklnk");
const tstring EXT_MKTASK = TEXT(".mktask");
const tstring EXT_MKTASK_INTERACTIVE = TEXT(".mktask.gui");

void usage() {
	MessageBox(NULL, TEXT("Usage: pdaemon <process-path>"), TEXT("pdaemon"), MB_OK);
}

int dispatch_mklnk(tstring self_path, LPTSTR* szArgList) {
	self_path = self_path.erase(self_path.find(EXT_MKLNK), EXT_MKLNK.length());
	return mklnk(self_path, szArgList[1]) ? 0 : 1;
}

int dispatch_mktask(tstring self_path, LPTSTR* szArgList) {
	self_path = self_path.erase(self_path.find(EXT_MKTASK), EXT_MKTASK.length());
	HRESULT hr = mktask(self_path, szArgList[1]);

	if (SUCCEEDED(hr)) {
		MessageBox(
			nullptr, 
			TEXT("Task created successfully."), 
			TEXT("pdaemon"), 
			MB_OK
		);
	}
	else {
		auto error_str = std::format(TEXT("Failed to create task. Error: {}"), hr);
		MessageBox(
			nullptr,
			error_str.c_str(),
			TEXT("pdaemon"),
			MB_OK
		);
	}

	return SUCCEEDED(hr) ? 0 : 1;
}

int dispatch_mktask_interactive(tstring self_path, LPTSTR* szArgList) {
	self_path = self_path.erase(self_path.find(EXT_MKTASK), EXT_MKTASK.length());
	HRESULT hr = mktask(self_path, szArgList[1]);

	SG_InputBox::GetString(
		TEXT("pdaemon"),
		TEXT("Task created successfully."),
		TEXT("OK")
	);

	if (SUCCEEDED(hr)) {
		MessageBox(
			nullptr,
			TEXT("Task created successfully."),
			TEXT("pdaemon"),
			MB_OK
		);
	}
	else {
		auto error_str = std::format(TEXT("Failed to create task. Error: {}"), hr);
		MessageBox(
			nullptr,
			error_str.c_str(),
			TEXT("pdaemon"),
			MB_OK
		);
	}

	return SUCCEEDED(hr) ? 0 : 1;
}

int dispatch_daemon(HINSTANCE hInstance, LPTSTR* szArgList) {
	auto context = DaemonContext{
			.process_path = szArgList[1],
	};
	worker(hInstance, context);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	LPTSTR* szArgList;
	int argCount;
	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);

#ifdef _DEBUG
	tstring self_path = szArgList[0];
	return mktask(
		self_path, TEXT("C:\\Windows\\System32\\notepad.exe")
	) ? 0 : 1;

#else
	if (szArgList == NULL || argCount != 2) {
		usage();
		return 0;
	}

	tstring self_path = szArgList[0];

	if (self_path.contains(EXT_MKLNK)) {
		return dispatch_mklnk(self_path, szArgList);
	}
	else if (self_path.contains(EXT_MKTASK)) {
		return dispatch_mktask(self_path, szArgList);
	}
	else if (self_path.contains(EXT_MKTASK_INTERACTIVE)) {
		return dispatch_mktask_interactive(self_path, szArgList);
	}
	else {
		return dispatch_daemon(hInstance, szArgList);
	}
#endif
}
