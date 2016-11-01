//
// Created by pav on 11.08.15.
//

#include <unordered_map>
#include <vector>
#include <cassert>
#include <cstring>

#include "filesystem.h"
#include "disk_file.h"

namespace gladius {
namespace core {
namespace filesystem {

std::unordered_map<c_file*, bool> g_files;
std::unordered_map<const char *, file_creator_t> g_devices;

bool init() {
    add_device(c_disk_file::DEVICE, c_disk_file::create);
    return true;
}

void shutdown() {
    g_devices.clear();
    for (auto &f : g_files) {
        if (f.first == nullptr)
            continue;
        delete f.first;
    }
}

void add_device(const char *device, file_creator_t creator) {
    g_devices[device] = creator;
}

bool parse_devices(const char *devices, std::vector<const char *> &devices_vector) {
    size_t len = std::char_traits<char>::length(devices);
    if (len == 0) {
        return false;
    }

    const char *device = devices;
    const char *end = devices + len;
    size_t i;
    while ((device < end) && (i = strcspn(device, ":")) <= std::char_traits<char>::length(device)) {
        for (const auto &p : g_devices) {
            if (std::char_traits<char>::length(p.first) != i)
                continue;
            if (memcmp(p.first, device, i) == 0) {
                devices_vector.push_back(p.first);
            }
        }
        device += (i + 1);
    }
    return !devices_vector.empty();
}

c_file* open(const char *devices, const char *filename, e_file_mode mode) {
    assert(g_devices.size() > 0);

    std::vector<const char *> devices_vector;
    if (!parse_devices(devices, devices_vector)) {
        return nullptr;
    }

    c_file *file = nullptr;
    c_file *new_file = nullptr;
    for (const auto device : devices_vector) {
        new_file = (*g_devices[device])(filename, file, mode);
        if (new_file == nullptr) {
            close(file);
            return nullptr;
        } else {
            file = new_file;
        }
    }

    g_files[file] = true;
    return file;
}

void close(c_file *file) {
    assert(file != nullptr);
    g_files.erase(file);
    delete file;
}

bool read(const char *devices, const char *filename, std::vector<char>& buffer) {
    c_file *file = open(devices, filename, e_file_mode::read);
    if (file == nullptr) {
        return false;
    }

    size_t size = file->size();
    buffer.resize(size);
    size_t r = file->read(&(buffer[0]), size);
    close(file);
    if (r == size) {
        return true;
    } else {
        return false;
    }
}

}
}
}
