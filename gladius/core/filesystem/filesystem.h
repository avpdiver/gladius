//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_FILESYSTEM_H
#define GLADIUS_FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include "file.h"

namespace gladius {
namespace core {
namespace filesystem {

typedef c_file *(*file_creator_t)(const char *filename, c_file *fallback, e_file_mode mode);

bool init();

void shutdown();

void add_device(const char *device, file_creator_t creator);

c_file *open(const char *devices, const char *filename, e_file_mode mode);

void close(c_file *file);

bool read(const char *devices, const char *filename, std::vector<char> &buffer);

}
}
}

#endif //GLADIUS_FILESYSTEM_H
