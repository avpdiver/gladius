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
            namespace c_filesystem
            {
                typedef c_file* (*file_creator_t)(const char* filename, c_file* fallback, e_file_mode mode);

                bool init();
                void shutdown();

                void add_device(const char* device, file_creator_t creator);

                handle_t open(const char* filename, e_file_mode mode);
                void close(handle_t file);

                size_t read(handle_t file, char* buf, size_t bytes);
                size_t write(handle_t file, const char* buf, size_t bytes);

                template <typename T>
                bool read(handle_t file, T& value)
                {
                    size_t actual = read(file, &value, sizeof(T));
                    if (actual == sizeof(T)) {
                        return true;
                    }
                    return false;
                }

                template <typename T>
                bool write(handle_t file, const T& value)
                {
                    size_t actual = write (file, &value, sizeof(T));
                    if (actual == sizeof(T)) {
                        return true;
                    }
                    return false;
                }
            }
        }
    }
}

#endif //GLADIUS_FILESYSTEM_H
