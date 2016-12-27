//
// Created by alexa on 027 27 12 2016.
//

#include "rasterization_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

VkPipelineRasterizationStateCreateInfo&& s_rasterization_state_desc::create() const {
    return std::move(*(VkPipelineRasterizationStateCreateInfo*)this);
}

}
}
}