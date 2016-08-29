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
            bool init()
            {
                return true;
            }

            void shutdown()
            {

            }

            void add_log(const char *format, ...)
            {
                /*char* buffer;
                va_list args;
                va_start (args, format);
                vsnprintf(buffer, 1, format, args);
                va_end (args);*/
            }
        }
    }
}