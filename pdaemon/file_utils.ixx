module;

#include <Windows.h>

export module file_utils;

import byte_utils;

export tstring basename(const tstring& path) {
	auto pos = path.find_last_of(TEXT("/\\"));
	if (pos == tstring::npos) {
		return path;
	}
	return path.substr(pos + 1);
}
