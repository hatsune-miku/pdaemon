#include <Windows.h>

export module byte_utils;

import <string>;

export std::string str_from_tchar(const TCHAR* tchar_str) {
	std::string ret;

#ifndef UNICODE
	ret = tchar_str;
#else 
	std::wstring wstr = tchar_str;
	ret = std::string(wstr.begin(), wstr.end());
#endif

	return ret;
}

export size_t tchar_from_str(const std::string& str, OUT TCHAR* tchar_str, size_t tchar_str_limit) {
	size_t ret = 0;

#ifndef UNICODE
	ret = str.copy(tchar_str, tchar_str_limit);
#else
	std::wstring wstr(str.begin(), str.end());
	ret = wstr.copy(tchar_str, tchar_str_limit);
#endif

	return ret;
}

export bool str_ends_with(const std::string& str, const std::string& suffix) {
	if (str.size() < suffix.size()) {
		return false;
	}

	return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
