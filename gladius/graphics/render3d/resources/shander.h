//
// Created by alexa on 028 28 12 2016.
//

#ifndef GLADIUS_SHANDER_H
#define GLADIUS_SHANDER_H


#include "../../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

DEFINE_HANDLE(shander_handle);

enum class e_shader_type {
    vertex,
    fragment
};

bool load_shader(const char *filename, e_shader_type shader_type, const char *entry,
                 shander_handle* handle);
void destroy_shader(shander_handle handle);

}
}
}
}

#endif //GLADIUS_SHANDER_H
