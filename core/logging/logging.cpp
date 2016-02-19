//
// Created by pav on 19.02.16.
//

#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <mutex>
#include "logging.h"

namespace gladius
{
    namespace core
    {
        namespace logging
        {
            const size_t BUFFER_SIZE = 1024;

            std::mutex mutex;
            std::vector<char*> errors;
            size_t current_position = 0;

            void add_error(const char *format, ...)
            {
                mutex.lock();
                char* buffer;

                if (current_position >= errors.size())
                {
                    buffer = new char[BUFFER_SIZE];
                    errors.push_back(buffer);
                }
                else
                {
                    buffer = errors[current_position++];
                }
                mutex.unlock();

                va_list args;
                va_start (args, format);
                vsnprintf(buffer, BUFFER_SIZE, format, args);
                va_end (args);
            }

            const char* get_error()
            {
                mutex.lock();
                char* buffer = errors[current_position];
                if (current_position > 0)
                {
                    current_position--;
                }
                mutex.unlock();
                return buffer;
            }
        }
    }
}