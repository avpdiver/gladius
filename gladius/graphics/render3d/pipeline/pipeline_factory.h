//
// Created by alexa on 029 29 12 2016.
//

#ifndef GLADIUS_PIPELINE_H
#define GLADIUS_PIPELINE_H

#include <vulkan/vulkan.h>
#include "vertex_type.h"
#include "../../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {

class c_pipeline_factory final {
private:
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state_create_info;
    VkPipelineRasterizationStateCreateInfo m_rasterization_state_create_info;
    VkPipelineMultisampleStateCreateInfo m_multisample_state_create_info;
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state_create_info;
    VkPipelineColorBlendStateCreateInfo m_color_blend_state_create_info;

public:
    c_pipeline_factory();
    NOT_COPYABLE(c_pipeline_factory);
    NOT_MOVEABLE(c_pipeline_factory);

public:
    bool create(VkRenderPass render_pass, uint32_t subpass, float width, float height,
                      const s_vertex_input_desc& vertex_input_desc, VkPipeline& pipeline);

private:
    bool create_input_assembly_state();
    bool create_rasterization_state();
    bool create_multisample_state();
    bool create_depth_stencil_state();
    bool create_color_blend_state();
    bool create_viewport_state(float width, float height,
                               VkViewport& viewport, VkRect2D& scissor,
                               VkPipelineViewportStateCreateInfo& viewport_create_info);
};

}
}
}

#endif //GLADIUS_PIPELINE_H
