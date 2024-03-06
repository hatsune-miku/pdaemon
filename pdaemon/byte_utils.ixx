export module byte_utils;

import <string>;

#ifdef UNICODE
export using tstring = std::wstring;
#else
export using tstring = std::string;
#endif
