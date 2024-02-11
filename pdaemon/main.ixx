import <format>;
import <iostream>;
import <string>;

import daemon;

void usage() noexcept {
	std::cerr << std::format("Usage: {} <process-path>", "pdaemon")
		<< std::endl;
}

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		usage();
		return 1;
	}

	auto context = DaemonContext{
		.process_path = argv[1]
	};
	worker(context);
	return 0;
}
