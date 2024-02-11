export module file_utils;

import <string>;

export std::string basename(const std::string& path) {
	auto pos = path.find_last_of("/\\");
	if (pos == std::string::npos) {
		return path;
	}
	return path.substr(pos + 1);
}
