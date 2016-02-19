//
// Created by pav on 19.02.16.
//

#ifndef GLADIUS_LOGGING_H
#define GLADIUS_LOGGING_H

#define SET_ERROR(format, ...) gladius::core::logging::add_error("%s(%d): " format, __FILENAME__, __LINE__, ##__VA_ARGS__)

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
