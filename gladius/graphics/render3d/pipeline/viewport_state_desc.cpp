//
// Created by alexa on 027 27 12 2016.
//

#include "pipeline_utils.h"
#include "viewport_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

VkViewport &&s_viewport_json::create() const {
    return std::move(*(VkViewport *) this);
}

VkRect2D&& s_scissor_json::create() const {
    VkRect2D info;
    info.offset.x = xOffset;
    info.offset.y = yOffset;
    info.extent.width = width;
    info.extent.height = height;
    return std::move(info);
}

bool s_viewport_state_json::create(s_viewport_state_desc& desc) const {
    desc.m_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    desc.m_create_info.pNext = nullptr;
    desc.m_create_info.flags = 0;

    if ((desc.m_create_info.viewportCount = viewports.size()) > 0) {
        for (const auto &v : viewports) {
            desc.m_viewports.push_back(v.create());
        }
        desc.m_create_info.pViewports = &(desc.m_viewports[0]);
    }

    if ((desc.m_create_info.scissorCount = scissors.size()) > 0) {
        for (const auto &s : scissors) {
            desc.m_scissors.push_back(s.create());
        }
        desc.m_create_info.pScissors = &(desc.m_scissors[0]);
    }

    return true;
}

}
}
}