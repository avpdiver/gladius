//
// Created by pav on 19.02.16.
//

#ifndef GLADIUS_LOGGING_H
#define GLADIUS_LOGGING_H

#define SET_ERROR(format, ...) gladius::core::logging::log(gladius::core::logging::e_log_level::error, "general", \
															__FILENAME__, __LINE__, format, __VA_ARGS__)

#define VERIFY_LOG(c, format, ...) 																				\
if (!(c)) {																										\
	gladius::core::logging::log(gladius::core::logging::e_log_level::error, "general", __FILENAME__, __LINE__, 	\
								format, __VA_ARGS__); 															\
	return false; 																								\
}

#define VERIFY(c) if (!(c)) return false;

namespace gladius {
namespace core {
namespace logging {

enum class e_log_level {
	fatal,
	error,
	warn,
	info,
	debug
};

bool init(e_log_level level);
void shutdown();

void log(e_log_level level, const char *type, const char* filename, int line, const char *format, ...);

}
}
}

#endif //GLADIUS_LOGGING_H
