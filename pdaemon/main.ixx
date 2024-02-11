#include <Windows.h>

import <format>;
import <string>;

import daemon;
import byte_utils;

void usage() {
	MessageBox(NULL, TEXT("Usage: pdaemon <process-path>"), TEXT("pdaemon"), MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	LPTSTR* szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
	if (szArgList == NULL || argCount != 2) {
		usage();
		return 0;
	}

	auto context = DaemonContext{
		.process_path = szArgList[1],
	};
	worker(context);
	return 0;
}
