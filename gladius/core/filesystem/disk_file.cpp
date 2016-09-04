//
// Created by pav on 12.08.15.
//

#include <cassert>
#include "filesystem.h"
#include "disk_file.h"

namespace gladius {
namespace core {
namespace filesystem {

c_file *c_disk_file::create(const char *filename, c_file *fallback, e_file_mode mode) {
    std::basic_ios<char> *stream = nullptr;
    switch (mode) {
        case e_file_mode::read:stream = new std::ifstream(filename, std::ios::binary);
            break;
        case e_file_mode::write:stream = new std::ofstream(filename, std::ios::binary);
            break;
        case e_file_mode::read_write:stream = new std::fstream(filename, std::ios::binary);
            break;
    }
    if (stream->fail()) {
        delete stream;
        return nullptr;
    }
    return new c_disk_file(stream, fallback, mode);
}

c_disk_file::~c_disk_file() {
    delete m_stream;
    if (m_fallback_file != nullptr) {
        delete m_fallback_file;
    }
}

size_t c_disk_file::read(char *buf, size_t bytes) {
    assert(m_mode == e_file_mode::read || m_mode == e_file_mode::read_write);
    return dynamic_cast<std::ifstream *>(m_stream)->readsome(buf, bytes);
}

size_t c_disk_file::write(const char *buf, size_t bytes) {
	assert(m_mode == e_file_mode::write || m_mode == e_file_mode::read_write);
    auto stream = dynamic_cast<std::ofstream *>(m_stream);
    auto before = stream->tellp();
    stream->write(buf, bytes);
    return stream->tellp() - before;
}

size_t c_disk_file::size() {
    auto s = dynamic_cast<std::ifstream *>(m_stream);
    size_t pos = s->tellg();
    s->seekg (0, s->end);
    size_t length = s->tellg();
    s->seekg (pos, s->beg);
    return length;
}

}
}
}