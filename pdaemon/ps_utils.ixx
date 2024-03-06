module;

#include <Windows.h>
#include <Psapi.h>

export module ps_utils;

import <string>;
import <stdexcept>;

import byte_utils;
import file_utils;

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
