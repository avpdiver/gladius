//
// Created by alexa on 004 04 09 2016.
//

#include "json_file.h"

namespace gladius {
namespace core {
namespace filesystem {

c_file *c_json_file::create(const char *filename, c_file *fallback, e_file_mode mode) {
    assert(fallback != nullptr);
    return new c_json_file(fallback, mode);
}

c_json_file::~c_json_file() {
    if (m_fallback_file != nullptr) {
        delete m_fallback_file;
    }
}

size_t c_json_file::size() {
    return m_fallback_file->size();
}

}
}
}