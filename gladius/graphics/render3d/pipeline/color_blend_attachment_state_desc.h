//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_COLOR_BLEND_ATTACHMENT_STATE_DESC_H
#define GLADIUS_COLOR_BLEND_ATTACHMENT_STATE_DESC_H

#include <vulkan/vulkan.h>

#include "../../../core/meta/json.h"
#include "../render3d_utils.h"
#include "pipeline_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_color_blend_attachment_state_desc : public VkPipelineColorBlendAttachmentState {
public:
    JSON_FIELDS(
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, blendEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, srcColorBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, dstColorBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, colorBlendOp, utils::string_to_blend_op),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, srcAlphaBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, dstAlphaBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, alphaBlendOp, utils::string_to_blend_op),
            JSON_FIELD_CONV(s_color_blend_attachment_state_desc, colorWriteMask, utils::string_to_color_component_flag)
    );

public:
    VkPipelineColorBlendAttachmentState&& create() const;
};

}
}
}

#endif //GLADIUS_COLOR_BLEND_ATTACHMENT_STATE_DESC_H
