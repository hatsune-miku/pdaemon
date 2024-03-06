module;

#include <Windows.h>
#include <Psapi.h>

export module ps_utils;

import <format>;
import <string>;
import <stdexcept>;

import byte_utils;
import file_utils;

export PROCESS_INFORMATION create_process(const tstring& process_path, const tstring& command_line) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	tstring full_command_line = command_line.empty()
		? process_path
		: std::format(TEXT("\"{}\" {}"), process_path, command_line);

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(
		process_path.c_str(),
		const_cast<LPTSTR>(full_command_line.c_str()),
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
