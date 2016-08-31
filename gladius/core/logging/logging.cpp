//
// Created by pav on 19.02.16.
//

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <memory>
#include <thread>
#include <fstream>
#include "logging.h"
#include "../collections/blocking_queue.h"

namespace gladius {
namespace core {
namespace logging {

e_log_level 		g_log_level;
std::atomic_flag 	g_is_running = ATOMIC_FLAG_INIT;
std::thread* 		g_writing_thread;

collections::c_blocking_queue<std::shared_ptr<std::string>> g_log_queue;
collections::c_blocking_queue<std::shared_ptr<std::string>> g_free_queue;

void log(e_log_level level, const char *type, const char *format, ...) {
	if (level > g_log_level) {
		return;
	}

	size_t len = std::char_traits<char>::length(type) << 1;

	std::shared_ptr<std::string> buffer;
	if (!g_free_queue.try_pop(buffer)) {
		buffer = std::shared_ptr<std::string>(new std::string());
	}

	va_list args;
	while (true) {
		buffer->resize(len);
		va_start (args, format);
		int w = std::vsnprintf(&(buffer->at(0)), buffer->size(), format, args);
		if (w > 0 && w < (int) buffer->size()) {
			break;
		}
		va_end (args);
		len = w + 1;
	}

	g_log_queue.push(buffer);
}


void write_log() {
	std::ofstream file("gladius.log");

	while (g_is_running.test_and_set()) {
		std::shared_ptr<std::string> buffer = g_log_queue.pop();
		file << *buffer;
		file.flush();
		g_free_queue.push(buffer);
	}

	file.flush();
	file.close();
}

bool init(e_log_level level) {
	g_log_level = level;
	g_is_running.test_and_set();
	g_writing_thread = new std::thread(write_log);
	return true;
}

void shutdown() {
	g_is_running.clear();
	g_log_queue.push(std::shared_ptr<std::string>(new std::string("Shutdown logging system.")));
	g_writing_thread->join();
}

}
}
}