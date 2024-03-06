export module byte_utils;

import <string>;

#ifdef _DEBUG
export using tstring = std::wstring;
#else
#ifdef UNICODE
export using tstring = std::wstring;
#else
export using tstring = std::string;
#endif
#endif
