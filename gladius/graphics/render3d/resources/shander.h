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


VkShaderModule load_shader(const char *filename, e_shader_type shader_type = e_shader_type::vertex,
                           const char *entry = nullptr, VkPipelineShaderStageCreateInfo *create_info = nullptr);
void destroy(VkShaderModule shader);

}
}
}
}

#endif //GLADIUS_SHANDER_H
