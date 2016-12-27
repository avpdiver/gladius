//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_MULTISAMPLE_STATE_DESC_H
#define GLADIUS_MULTISAMPLE_STATE_DESC_H

#include <vulkan/vulkan.h>
#include "../../../core/meta/json.h"
#include "pipeline_utils.h"
#include "../render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_multisample_state_desc : public VkPipelineMultisampleStateCreateInfo {
public:
    JSON_FIELDS(
            JSON_FIELD_CONV(s_multisample_state_desc, rasterizationSamples, utils::get_sample_count),
            JSON_FIELD_CONV(s_multisample_state_desc, sampleShadingEnable, bool_to_uint32),
            JSON_FIELD(s_multisample_state_desc, minSampleShading),
            JSON_FIELD_CONV(s_multisample_state_desc, alphaToCoverageEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_multisample_state_desc, alphaToOneEnable, bool_to_uint32)
    );

public:
    s_multisample_state_desc() {
        sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pNext = nullptr;
        flags = 0;
    }
public:
    VkPipelineMultisampleStateCreateInfo &&create() const;
};

}
}
}

#endif //GLADIUS_MULTISAMPLE_STATE_DESC_H
