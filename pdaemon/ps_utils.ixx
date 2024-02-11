module;

#include <Windows.h>
#include <Psapi.h>

export module ps_utils;

import <string>;
import <stdexcept>;

import byte_utils;
import file_utils;

export bool is_process_exist(const tstring& process_path) {
	DWORD dwProcessIds[1024];
	DWORD dwCpNeeded;
	if (!EnumProcesses(dwProcessIds, sizeof(dwProcessIds), &dwCpNeeded)) {
		throw std::exception("EnumProcesses failed");
	}

	DWORD dwcProcesses = dwCpNeeded / sizeof(DWORD);
	for (size_t i = 0; i < dwcProcesses; ++i) {
		DWORD dwProcessId = dwProcessIds[i];
		if (dwProcessId == 0) {
			continue;
		}

		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION,
			FALSE,
			dwProcessId
		);

		if (hProcess == NULL) {
			continue;
		}

		TCHAR szImageFileName[MAX_PATH] = TEXT("<unknown>");
		GetProcessImageFileName(hProcess, szImageFileName, sizeof(szImageFileName));

		tstring image_file_path = szImageFileName;
		if (basename(process_path) == basename(image_file_path)) {
			CloseHandle(hProcess);
			return true;
		}
	}
	return false;
}

export PROCESS_INFORMATION create_process(const tstring& process_path) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(
		process_path.c_str(),
		NULL,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi
	)) {
		throw std::exception("CreateProcess failed");
	}
	return pi;
}
