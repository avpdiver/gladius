//
// Created by alexa on 027 27 12 2016.
//

#include "multisample_state_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

VkPipelineMultisampleStateCreateInfo&& s_multisample_state_desc::create() const {
    return std::move(*(VkPipelineMultisampleStateCreateInfo*)this);
}

}
}
}