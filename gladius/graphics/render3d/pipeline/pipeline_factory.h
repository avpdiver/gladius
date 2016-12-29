//
// Created by alexa on 029 29 12 2016.
//

#ifndef GLADIUS_PIPELINE_H
#define GLADIUS_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../../../core/types.h"
#include "../resources/texture.h"
#include "../renderer3d.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_pipeline {
    resources::texture_handle m_depth_stencil;
    VkPipeline m_pipeline;
    VkRenderPass m_render_pass;
    VkPipelineLayout m_pipeline_layout;
    VkDescriptorSetLayout m_descriptor_set_layout;
    std::vector<VkFramebuffer> m_framebuffers;
};

bool create_forward_rendering_pipeline(c_renderer3d* renderer, s_pipeline& pipeline);

}
}
}

#endif //GLADIUS_PIPELINE_H
