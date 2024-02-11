export module byte_utils;

import <string>;

#ifndef UNICODE
export using tstring = std::string;
#else
export using tstring = std::wstring;
#endif
