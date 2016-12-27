//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_PIPELINE_DESC_H
#define GLADIUS_PIPELINE_DESC_H

#include <vector>
#include "../render3d_resources.h"
#include "viewport_state_desc.h"
#include "subpass_desc.h"
#include "renderpass_desc.h"
#include "framebuffer_desc.h"
#include "color_blend_state_desc.h"
#include "multisample_state_desc.h"
#include "rasterization_state_desc.h"
#include "../render3d_swapchain.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_pipelines_desc {
public:
    std::vector <s_viewport_state_desc> m_viewport_states;
    std::vector <VkPipelineRasterizationStateCreateInfo> m_rasterization_states;
    std::vector <s_color_blend_state_desc> m_color_blend_states;
    std::vector <VkPipelineMultisampleStateCreateInfo> m_multisample_states;

    std::vector<VkRenderPass> m_render_passes;
    std::vector<resources::framebuffer_handle> m_framebuffers;

    s_swapchain_desc m_swapchain_desc;

    std::vector <VkPipelineShaderStageCreateInfo> m_stages;
    std::vector <VkPipelineVertexInputStateCreateInfo> m_vertex_input_states;
    std::vector <VkPipelineInputAssemblyStateCreateInfo> m_input_assembly_states;

    std::vector <VkPipelineDepthStencilStateCreateInfo> m_depth_stencil_states;
    std::vector <VkPipelineTessellationStateCreateInfo> m_tessellation_states;
    std::vector <VkPipelineDynamicStateCreateInfo> m_dynamic_states;

    std::vector <VkGraphicsPipelineCreateInfo> m_pipeline_create_infos;

public:
    bool create() const;
};

struct s_graphics_pipeline_json {
    ssize_t vertexInputState;
    ssize_t inputAssemblyState;
    ssize_t viewportState;
    ssize_t rasterizationState;
    ssize_t multisampleState;
    ssize_t depthStencilState;
    ssize_t colorBlendState;
    ssize_t dynamicState;
    size_t renderPass;
    size_t subpass;

    JSON_FIELDS(
            JSON_FIELD(s_graphics_pipeline_json, vertexInputState),
            JSON_FIELD(s_graphics_pipeline_json, inputAssemblyState),
            JSON_FIELD(s_graphics_pipeline_json, viewportState),
            JSON_FIELD(s_graphics_pipeline_json, rasterizationState),
            JSON_FIELD(s_graphics_pipeline_json, multisampleState),
            JSON_FIELD(s_graphics_pipeline_json, depthStencilState),
            JSON_FIELD(s_graphics_pipeline_json, colorBlendState),
            JSON_FIELD(s_graphics_pipeline_json, dynamicState),
            JSON_FIELD(s_graphics_pipeline_json, renderPass),
            JSON_FIELD(s_graphics_pipeline_json, subpass)
    );
};

struct s_pipelines_json {
public:
    std::vector<s_viewport_state_json> viewport_states;
    std::vector<s_rasterization_state_desc> rasterization_states;
    std::vector<s_multisample_state_desc> multisample_states;
    std::vector<s_color_blend_state_json> color_blend_states;
    std::vector<s_renderpass_json> renderpasses;
    std::vector<s_graphics_pipeline_json> graphics_pipelines;
    s_swapchain_json swapchain;
    std::vector<s_framebuffer_json> framebuffers;

    JSON_FIELDS(
            JSON_FIELD(s_pipelines_json, swapchain),
            JSON_FIELD(s_pipelines_json, viewport_states),
            JSON_FIELD(s_pipelines_json, rasterization_states),
            JSON_FIELD(s_pipelines_json, multisample_states),
            JSON_FIELD(s_pipelines_json, color_blend_states),
            JSON_FIELD(s_pipelines_json, renderpasses),
            JSON_FIELD(s_pipelines_json, framebuffers)
    );

public:
    bool create(s_pipelines_desc& desc) const;
};

}
}
}

#endif //GLADIUS_PIPELINE_DESC_H
