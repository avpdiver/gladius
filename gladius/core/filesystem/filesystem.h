//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_FILESYSTEM_H
#define GLADIUS_FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include <initializer_list>
#include "file.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            typedef c_file* (*file_creator_t)(const char* filename, c_file* fallback);

            namespace c_filesystem
            {
                void add_device(const char* device, file_creator_t creator);

                c_file* open(const char* filename, std::initializer_list<const char*> devices);
                void close(c_file* file);
            }
        }
    }
}

#endif //GLADIUS_FILESYSTEM_H
