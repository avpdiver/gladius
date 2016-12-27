//
// Created by alexa on 027 27 12 2016.
//

#include "color_blend_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool s_color_blend_state_json::create(s_color_blend_state_desc& desc) const {

    desc.m_create_info = {};

    desc.m_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    desc.m_create_info.pNext = nullptr;
    desc.m_create_info.flags = 0;

    desc.m_create_info.logicOpEnable = logicOpEnable;
    desc.m_create_info.logicOp = logicOp;
    if (blendConstants.size() == 0) {
        desc.m_create_info.blendConstants[0]
                = desc.m_create_info.blendConstants[1]
                = desc.m_create_info.blendConstants[2]
                = desc.m_create_info.blendConstants[3] = 0.0f;
    } else if (blendConstants.size() == 1) {
        desc.m_create_info.blendConstants[0]
                = desc.m_create_info.blendConstants[1]
                = desc.m_create_info.blendConstants[2]
                = desc.m_create_info.blendConstants[3] = blendConstants[0];
    } else {
        size_t i = 0;
        for (auto &f : blendConstants) {
            desc.m_create_info.blendConstants[i++] = f;
        }
    }

    if ((desc.m_create_info.attachmentCount = color_blend_attachment_states.size()) > 0) {
        for (const auto &a : color_blend_attachment_states) {
            desc.m_color_blend_attachment_states.push_back(a.create());
        }
        desc.m_create_info.pAttachments = &(desc.m_color_blend_attachment_states[0]);
    }

    return true;
}

}
}
}