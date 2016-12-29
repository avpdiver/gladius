//
// Created by alexa on 029 29 12 2016.
//

#ifndef GLADIUS_VERTEX_TYPE_H
#define GLADIUS_VERTEX_TYPE_H

#include <vector>
#include <cstdint>
#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {

enum class e_vertex_component {
    position = 0,
    normal = 1,
    color = 2,
    uv = 3,
    tangent = 4,
    bitangent = 5,
    dummy_float = 6,
    dummy_vec4 = 7
};

struct s_vertex_input_desc {
    std::vector<VkVertexInputBindingDescription> m_vertex_input_binding_descs;
    std::vector<VkVertexInputAttributeDescription> m_vertex_input_attribute_descs;
    VkPipelineVertexInputStateCreateInfo m_vertex_input_state_create_info;

    s_vertex_input_desc(const std::vector<std::vector<e_vertex_component>>& vertex_desc);
};

}
}
}

#endif //GLADIUS_VERTEX_TYPE_H
