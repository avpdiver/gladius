//
// Created by alexa on 027 27 12 2016.
//

#include "color_blend_attachment_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

VkPipelineColorBlendAttachmentState&& s_color_blend_attachment_state_desc::create() const {
    return std::move(*(VkPipelineColorBlendAttachmentState*)this);
}

}
}
}