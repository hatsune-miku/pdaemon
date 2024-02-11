#include <Windows.h>

export module daemon;

import <string>;
import <vector>;
import <iostream>;

import byte_utils;
import ps_utils;

export struct DaemonContext {
	std::string process_path;
};

export inline void msleep(DWORD dwMilliseconds) {
	Sleep(dwMilliseconds);
}

export void worker(const DaemonContext& context) noexcept {
	while (true) {
		msleep(1000);

		try {
			if (!is_process_exist(context.process_path)) {
				create_process(context.process_path);
			}
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}
