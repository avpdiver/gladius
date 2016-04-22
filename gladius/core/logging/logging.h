//
// Created by pav on 19.02.16.
//

#ifndef GLADIUS_LOGGING_H
#define GLADIUS_LOGGING_H

#define SET_ERROR(error) gladius::core::logging::add_error("%s(%d): " error, __FILENAME__, __LINE__)
#define SET_ERRORF(format, ...) gladius::core::logging::add_error("%s(%d): " format, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define VERIFY_LOG(c, error)  if (!(c)) {gladius::core::logging::add_error("%s(%d): " error, __FILENAME__, __LINE__); return false;}
#define VERIFY_LOGF(c, format, ...)  if (!(c)) {gladius::core::logging::add_error("%s(%d): " format, __FILENAME__, __LINE__, ##__VA_ARGS__); return false;}
#define VERIFY(c) if (!(c)) return false;

namespace gladius
{
    namespace core
    {
        namespace logging
        {
            void add_error(const char *format, ...);
            const char* get_error();
        }
    }
}

#endif //GLADIUS_LOGGING_H
