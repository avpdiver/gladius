//
// Created by alexa on 004 04 09 2016.
//

#ifndef GLADIUS_JSON_FILE_H
#define GLADIUS_JSON_FILE_H

#include <stddef.h>
#include <fstream>
#include <cassert>
#include "file.h"
#include "../json.h"

namespace gladius {
namespace core {
namespace filesystem {

class c_json_file : public c_file {
public:
    static constexpr char const *DEVICE = "json";

public:
    static c_file *create(const char *filename, c_file *fallback, e_file_mode mode);

private:
    c_json_file(c_file *fallback, e_file_mode mode) :
            m_fallback_file(fallback), m_mode(mode) {}

public:
    ~c_json_file();

public:
    template <typename TYPE>
    bool read(TYPE& value);

private:
    virtual size_t read(char *buf, size_t bytes) { return 0; }
    virtual size_t write(const char *buf, size_t bytes) { return 0; }

public:
    virtual size_t size();

private:
    c_file *m_fallback_file;
    e_file_mode m_mode;
};

template <typename TYPE>
bool c_json_file::read(TYPE& value) {
    assert(m_mode == e_file_mode::read || m_mode == e_file_mode::read_write);

    size_t size = m_fallback_file->size();
    std::string json;
    json.resize(size);
    m_fallback_file->read(&json[0], size);
    auto j = nlohmann::json::parse(json);
    json::from_json(value, j);
    return true;
}

}
}
}

#endif //GLADIUS_JSON_FILE_H
