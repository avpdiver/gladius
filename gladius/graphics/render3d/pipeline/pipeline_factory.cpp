//
// Created by alexa on 029 29 12 2016.
//

#include "pipeline_factory.h"
#include "../render3d_macros.h"
#include "../render3d_debug.h"
#include "../render3d_globals.h"

namespace gladius {
namespace graphics {
namespace render3d {

c_pipeline_factory::c_pipeline_factory() {
    create_input_assembly_state();
    create_rasterization_state();
    create_multisample_state();
    create_depth_stencil_state();
    create_color_blend_state();
}

bool c_pipeline_factory::create_input_assembly_state() {
    m_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_input_assembly_state_create_info.pNext = nullptr;
    m_input_assembly_state_create_info.flags = 0;
    m_input_assembly_state_create_info.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_input_assembly_state_create_info.primitiveRestartEnable = VK_TRUE;
    return true;
}

bool c_pipeline_factory::create_rasterization_state() {
    m_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterization_state_create_info.pNext = nullptr;
    m_rasterization_state_create_info.flags = 0;
    m_rasterization_state_create_info.depthClampEnable = VK_FALSE;
    m_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    m_rasterization_state_create_info.polygonMode =  VkPolygonMode::VK_POLYGON_MODE_FILL;
    m_rasterization_state_create_info.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
    m_rasterization_state_create_info.frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    m_rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    m_rasterization_state_create_info.depthBiasClamp = 0.0f;
    m_rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
    m_rasterization_state_create_info.lineWidth = 1.0f;
    return true;
}

bool c_pipeline_factory::create_multisample_state() {
    m_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multisample_state_create_info.pNext = nullptr;
    m_multisample_state_create_info.flags = 0;
    m_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    m_multisample_state_create_info.minSampleShading = 1.0f;
    m_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    m_multisample_state_create_info.alphaToOneEnable = VK_FALSE;
    m_multisample_state_create_info.pSampleMask = nullptr;
    return true;
}

bool c_pipeline_factory::create_depth_stencil_state() {
    m_depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depth_stencil_state_create_info.pNext = nullptr;
    m_depth_stencil_state_create_info.flags = 0;
    m_depth_stencil_state_create_info.depthTestEnable = VK_TRUE;
    m_depth_stencil_state_create_info.depthWriteEnable = VK_TRUE;
    m_depth_stencil_state_create_info.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS;
    m_depth_stencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
    m_depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
    //m_depth_stencil_state_create_info.front;
    //m_depth_stencil_state_create_info.back;
    m_depth_stencil_state_create_info.minDepthBounds = 0.0f;
    m_depth_stencil_state_create_info.maxDepthBounds = 1.0f;
    return true;
}

bool c_pipeline_factory::create_color_blend_state() {
    m_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_color_blend_state_create_info.pNext = nullptr;
    m_color_blend_state_create_info.flags = 0;

    m_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    m_color_blend_state_create_info.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
    m_color_blend_state_create_info.attachmentCount = 0;
    m_color_blend_state_create_info.pAttachments = nullptr;
    m_color_blend_state_create_info.blendConstants[0]
            = m_color_blend_state_create_info.blendConstants[1]
            = m_color_blend_state_create_info.blendConstants[2]
            = m_color_blend_state_create_info.blendConstants[3] = 0.0f;
    return true;
}

bool c_pipeline_factory::create_viewport_state(float width, float height,
                                               VkViewport& viewport, VkRect2D& scissor,
                                               VkPipelineViewportStateCreateInfo& viewport_create_info) {

    viewport.x = 0;
    viewport.y = 0;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = (uint32_t)width;
    scissor.extent.height = (uint32_t)height;

    viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_create_info.pNext = nullptr;
    viewport_create_info.flags = 0;
    viewport_create_info.viewportCount = 1;
    viewport_create_info.pViewports = &viewport;
    viewport_create_info.scissorCount = 1;
    viewport_create_info.pScissors = &scissor;

    return true;
}

bool c_pipeline_factory::create(VkRenderPass render_pass, uint32_t subpass, float width, float height,
                                      const s_vertex_input_desc& vertex_input_desc,
                                VkPipeline& pipeline) {

    VkPipelineLayoutCreateInfo layout_info = {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0,
            0, nullptr,
            0, nullptr
    };

    VkPipelineLayout pipelineLayout;
    VK_VERIFY (vkCreatePipelineLayout(vk_globals::device, &layout_info, nullptr, &pipelineLayout));

    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewport_state_create_info;
    VERIFY(create_viewport_state(width, height, viewport, scissor, viewport_state_create_info));

    VkGraphicsPipelineCreateInfo create_info = {
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0,
            0, nullptr,
            &vertex_input_desc.m_vertex_input_state_create_info,
            &m_input_assembly_state_create_info,
            nullptr,
            &viewport_state_create_info,
            &m_rasterization_state_create_info,
            &m_multisample_state_create_info,
            &m_depth_stencil_state_create_info,
            &m_color_blend_state_create_info,
            nullptr,
            pipelineLayout,
            render_pass, subpass,
            VK_NULL_HANDLE, -1
    };

    VK_VERIFY(vkCreateGraphicsPipelines(vk_globals::device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline));

    return true;
}

}
}
}