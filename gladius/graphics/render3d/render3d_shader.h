//
// Created by alexa on 004 04 09 2016.
//

#ifndef GLADIUS_RENDER3D_SHADER_H
#define GLADIUS_RENDER3D_SHADER_H

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

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

#endif //GLADIUS_RENDER3D_SHADER_H
