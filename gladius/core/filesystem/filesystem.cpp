//
// Created by pav on 11.08.15.
//

#include <unordered_map>
#include <iterator>
#include <vector>
#include <cstring>
#include <cassert>
#include "filesystem.h"
#include "disk_file.h"

namespace gladius {
namespace core {
namespace filesystem {

const size_t MAX_FILES_NUMBER = 1024;
c_file *g_files[MAX_FILES_NUMBER] = {nullptr};

std::unordered_map<const char *, file_creator_t> g_devices;

bool init() {
    add_device(c_disk_file::DEVICE, c_disk_file::create);
    return true;
}

void shutdown() {
    g_devices.clear();
    for (auto &f : g_files) {
        if (f == nullptr)
            continue;
        delete f;
        f = nullptr;
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

handle_t get_free_slot() {
    for (size_t i = 0; i < MAX_FILES_NUMBER; ++i) {
        if (g_files[i] == nullptr) {
            return i;
        }
    }
    return INVALID_HANDLE;
}

handle_t open(const char *devices, const char *filename, e_file_mode mode) {
    assert(g_devices.size() > 0);

    handle_t file_handle = get_free_slot();
    if (file_handle == INVALID_HANDLE) {
        return INVALID_HANDLE;
    }

    std::vector<const char *> devices_vector;
    if (!parse_devices(devices, devices_vector)) {
        return INVALID_HANDLE;
    }

    c_file *file = nullptr;
    c_file *new_file = nullptr;
    for (const auto device : devices_vector) {
        g_files[file_handle] = file;
        new_file = (*g_devices[device])(filename, file, mode);
        if (new_file == nullptr) {
            close(file_handle);
            file_handle = INVALID_HANDLE;
            break;
        } else {
            file = new_file;
        }
    }

    return file_handle;
}

void close(handle_t file) {
    assert(file < MAX_FILES_NUMBER && g_files[file] != nullptr);
    delete g_files[file];
}

size_t read(handle_t file, char *buf, size_t bytes) {
    assert(file < MAX_FILES_NUMBER && g_files[file] != nullptr);
    return g_files[file]->read(buf, bytes);
}

size_t write(handle_t file, const char *buf, size_t bytes) {
    assert(file < MAX_FILES_NUMBER && g_files[file] != nullptr);
    return g_files[file]->write(buf, bytes);
}

}
}
}
