//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_FILE_H
#define GLADIUS_FILE_H

#include "../types.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            enum class e_file_mode
            {
                read,
                write,
                read_write
            };

            class c_file
            {
            public:
                virtual ~c_file() {}

            public:
                virtual size_t read(char* buf, size_t bytes) = 0;
                virtual size_t write(const char* buf, size_t bytes) = 0;
            };
        }
    }
}

#endif //GLADIUS_FILE_H
