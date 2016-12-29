//
// Created by alexa on 029 29 12 2016.
//

#include "vertex_type.h"

namespace gladius {
namespace graphics {
namespace render3d {

uint32_t get_component_size(e_vertex_component component) {
    switch (component) {
        case e_vertex_component::uv:
            return 2 * sizeof(float);
        case e_vertex_component::dummy_float:
            return sizeof(float);
        case e_vertex_component::dummy_vec4:
            return 4 * sizeof(float);
        default:
            return 3 * sizeof(float);
    }
}

VkFormat get_component_format(e_vertex_component component) {
    switch (component) {
        case e_vertex_component::uv:
            return VK_FORMAT_R32G32_SFLOAT;
        case e_vertex_component::dummy_float:
            return VK_FORMAT_UNDEFINED;
        case e_vertex_component::dummy_vec4:
            return VK_FORMAT_UNDEFINED;
        default:
            return VK_FORMAT_R32G32B32_SFLOAT;
    }
}

uint32_t get_vertex_size(const std::vector<e_vertex_component>& components) {
    uint32_t size = 0;
    for (const auto& c : components) {
        size += get_component_size(c);
    }
    return size;
}

s_vertex_input_desc::s_vertex_input_desc(const std::vector<std::vector<e_vertex_component>>& vertex_desc) {

    uint32_t location = 0;
    uint32_t binding = 0;
    for (const auto& b : vertex_desc) {
        m_vertex_input_binding_descs.push_back({
                                                       binding,
                                                       get_vertex_size(b),
                                                       VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
                                               });

        uint32_t offset = 0;
        for (const auto &c : b) {
            auto format = get_component_format(c);
            auto size = get_component_size(c);
            if (format != VK_FORMAT_UNDEFINED) {
                m_vertex_input_attribute_descs.push_back({
                                                                 location++,
                                                                 binding,
                                                                 format,
                                                                 offset
                                                         });
            }
            offset += size;
        }

        binding++;
    }

    m_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertex_input_state_create_info.pNext = nullptr;
    m_vertex_input_state_create_info.flags = 0;
    m_vertex_input_state_create_info.vertexBindingDescriptionCount = (uint32_t) m_vertex_input_binding_descs.size();
    m_vertex_input_state_create_info.pVertexBindingDescriptions = &(m_vertex_input_binding_descs[0]);
    m_vertex_input_state_create_info.vertexAttributeDescriptionCount = (uint32_t) m_vertex_input_attribute_descs.size();
    m_vertex_input_state_create_info.pVertexAttributeDescriptions = &(m_vertex_input_attribute_descs[0]);

}

}
}
}