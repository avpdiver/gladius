//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_RASTERIZATION_STATE_DESC_H
#define GLADIUS_RASTERIZATION_STATE_DESC_H

#include <vulkan/vulkan.h>
#include "../../../core/meta/json.h"
#include "pipeline_utils.h"
#include "../render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_rasterization_state_desc : public VkPipelineRasterizationStateCreateInfo {
public:
    JSON_FIELDS(
            JSON_FIELD_CONV(s_rasterization_state_desc, depthClampEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_rasterization_state_desc, rasterizerDiscardEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_rasterization_state_desc, polygonMode, utils::string_to_polygon_mode),
            JSON_FIELD_CONV(s_rasterization_state_desc, cullMode, utils::string_to_cull_mode),
            JSON_FIELD_CONV(s_rasterization_state_desc, frontFace, utils::string_to_front_face),
            JSON_FIELD_CONV(s_rasterization_state_desc, depthBiasEnable, bool_to_uint32),
            JSON_FIELD(s_rasterization_state_desc, depthBiasConstantFactor),
            JSON_FIELD(s_rasterization_state_desc, depthBiasClamp),
            JSON_FIELD(s_rasterization_state_desc, depthBiasSlopeFactor),
            JSON_FIELD(s_rasterization_state_desc, lineWidth)
    );

public:
    s_rasterization_state_desc() {
        sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pNext = nullptr;
        flags = 0;
    }

public:
    VkPipelineRasterizationStateCreateInfo&& create() const;
};

}
}
}

#endif //GLADIUS_RASTERIZATION_STATE_DESC_H
