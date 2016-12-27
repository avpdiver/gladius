//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_COLOR_BLEND_STATE_DESC_H
#define GLADIUS_COLOR_BLEND_STATE_DESC_H

#include <vulkan/vulkan.h>
#include "../../../core/meta/json.h"
#include "color_blend_attachment_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_color_blend_state_desc {
    VkPipelineColorBlendStateCreateInfo m_create_info;
    std::vector <VkPipelineColorBlendAttachmentState> m_color_blend_attachment_states;
};

struct s_color_blend_state_json {
    bool logicOpEnable = false;
    VkLogicOp logicOp = VK_LOGIC_OP_COPY;
    std::vector<float> blendConstants {0.0, 0.0, 0.0, 0.0};
    std::vector<s_color_blend_attachment_state_desc> color_blend_attachment_states;

    JSON_FIELDS(
            JSON_FIELD(s_color_blend_state_json, logicOpEnable),
            JSON_FIELD_CONV(s_color_blend_state_json, logicOp, utils::string_to_logic_op),
            JSON_FIELD(s_color_blend_state_json, blendConstants),
            JSON_FIELD(s_color_blend_state_json, color_blend_attachment_states)
    );

public:
    bool create(s_color_blend_state_desc& desc) const;
};

}
}
}

#endif //GLADIUS_COLOR_BLEND_STATE_DESC_H
