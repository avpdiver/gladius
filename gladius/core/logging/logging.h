//
// Created by pav on 19.02.16.
//

#ifndef GLADIUS_LOGGING_H
#define GLADIUS_LOGGING_H

#define SET_ERROR(text) gladius::core::logging::log(gladius::core::logging::e_log_level::error, "%s(%d): " text, __FILENAME__, __LINE__)
#define SET_ERRORF(format, ...) gladius::core::logging::log(gladius::core::logging::e_log_level::error, "%s(%d): " format, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define VERIFY_LOG(c, text)  if (!(c)) {gladius::core::logging::log(gladius::core::logging::e_log_level::error, "%s(%d): " text, __FILENAME__, __LINE__); return false;}
#define VERIFY_LOGF(c, format, ...)  if (!(c)) {gladius::core::logging::log(gladius::core::logging::e_log_level::error, "%s(%d): " format, __FILENAME__, __LINE__, ##__VA_ARGS__); return false;}
#define VERIFY(c) if (!(c)) return false;

namespace gladius {
namespace core {
namespace logging {

enum class e_log_level {
    fatal,
    error,
    warn,
    debug,
    info
};

bool init();
void shutdown();

void log(e_log_level level, const char *type, const char *format, ...);

}
}
}

#endif //GLADIUS_LOGGING_H
