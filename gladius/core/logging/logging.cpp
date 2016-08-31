//
// Created by pav on 19.02.16.
//

#include <stdio.h>
#include <string>
#include <memory>
#include <thread>
#include <fstream>
#include "logging.h"
#include "../collections/blocking_queue.h"

namespace gladius {
namespace core {
namespace logging {

const char* 	HEAD 	=
	"<html>\n"
		"<head>\n"
		"<style>\n"
		"   table {\n"
		"    border: 4px double #333;\n"
		"    border-collapse: separate;\n"
		"    width: 100%;\n"
		"    border-spacing: 3px 3px;\n"
		"   }\n"
		"   td {\n"
		"    padding: 5px;\n"
		"    border: 1px solid #a52a2a;\n"
		"   }\n"
		"  </style>\n"
		"</head>\n"
		"<body>\n"
		"<table style='border: 1px'>\n"
		"<tr><th>TIME</th><th>TYPE</th><th>LEVEL</th><th>FILE</th><th>LINE</th><th>DESC</th></tr>\n";

const char* 	TAIL 	= "</table></body></html>";
const char* 	ROW 	= "<tr style='background-color:%s'><td>%i</td><td>%s</td><td>%s</td><td>%s</td><td>%i</td><td>%s</td></tr>\n";
const size_t 	ROW_LEN = std::char_traits<char>::length(ROW) + 7; // 7 for web color format #rrggbb

const std::array<const char*, ((size_t)(e_log_level::debug) + 1)> LEVELS {{ "FATAL", "ERROR", "WARN", "INFO", "DEBUG" }};
const std::array<const char*, ((size_t)(e_log_level::debug) + 1)> LEVELS_COLORS {{ "#ff0000", "#ff4000", "#ffff00", "#009800", "#0000ff" }};

const auto 		START_TIME = std::chrono::system_clock::now();

e_log_level 		g_log_level;
std::atomic_flag 	g_is_running = ATOMIC_FLAG_INIT;
std::thread* 		g_writing_thread;
std::ofstream 		g_file;
thread_local		std::string g_local_buffer;

collections::c_blocking_queue<std::shared_ptr<std::string>> g_log_queue;
collections::c_blocking_queue<std::shared_ptr<std::string>> g_free_queue;

void log(e_log_level level, const char *type, const char* filename, int line, const char *format, ...) {
	if (level > g_log_level) {
		return;
	}

	g_local_buffer.clear();

	size_t len = std::char_traits<char>::length(format) << 1;

	std::shared_ptr<std::string> buffer;
	if (!g_free_queue.try_pop(buffer)) {
		buffer = std::shared_ptr<std::string>(new std::string());
	}

	va_list args;
	while (true) {
		g_local_buffer.resize(len);
		va_start (args, format);
		int w = std::vsnprintf(&(g_local_buffer.at(0)), g_local_buffer.length(), format, args);
		va_end (args);
		if (w > 0 && w < (int) g_local_buffer.size()) {
			auto now = std::chrono::system_clock::now();
			auto time = now - START_TIME;

			len = 10  										// space for time.count())
				+ std::char_traits<char>::length(type)
				+ std::char_traits<char>::length(LEVELS[(size_t)level])
				+ std::char_traits<char>::length(filename)
				+ g_local_buffer.length()
				+ ROW_LEN;

			buffer->resize(len);
			w = std::snprintf(&(buffer->at(0)), buffer->length(), ROW, LEVELS_COLORS[(size_t)level], time.count(), type,
							  LEVELS[(size_t)level], filename, line, g_local_buffer.c_str());
			buffer->resize(w);

			break;
		}
		len = w + 1;
	}

	g_log_queue.push(buffer);
}


void write_log() {
	while (g_is_running.test_and_set()) {
		std::shared_ptr<std::string> buffer = g_log_queue.pop();
		g_file << *buffer << '\n';
		g_file.flush();
		g_free_queue.push(buffer);
	}
	g_file << TAIL;
	g_file.flush();
	g_file.close();
}

bool init(e_log_level level) {
	g_log_level = level;
	g_is_running.test_and_set();
	g_file.open("gladius.html");
	g_file << HEAD;
	g_writing_thread = new std::thread(write_log);
	return true;
}

void shutdown() {
	g_is_running.clear();
	log(e_log_level::info, "GENERAL", __FILENAME__, __LINE__, "Shutdown logging system");
	g_writing_thread->join();
}

}
}
}