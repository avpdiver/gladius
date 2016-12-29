//
// Created by alexa on 029 29 12 2016.
//

#include "pipeline_factory.h"
#include "../vulkan/vulkan_macros.h"
#include "../vulkan/vulkan_debug.h"
#include "../renderer3d.h"
#include "../resources/shander.h"
#include "vertex_type.h"
#include "../vulkan/vulkan_utils.h"
#include "../resources/texture.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool create_forward_depth_stencil(c_renderer3d* renderer, s_pipeline& pipeline) {
    VkFormat format;
    VERIFY(utils::get_supported_depth_format(renderer->m_gpu, &format));
    return resources::create_texture(format,
                                     renderer->m_surface.capabilities.currentExtent.width,
                                     renderer->m_surface.capabilities.currentExtent.height,
                                     1,
                                     1, 1, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
                                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                     &pipeline.m_depth_stencil);
}

bool create_forward_render_pass(c_renderer3d* renderer, s_pipeline& pipeline) {
    std::array<VkAttachmentDescription, 2> attachments = {};

    // Color attachment
    attachments[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;									// We don't use multi sampling in this example
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear this attachment at the start of the render pass
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							// Keep it's contents after the render pass is finished (for displaying it)
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// We don't use stencil, so don't care for load
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				// Same for store
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						// Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					// Layout to which the attachment is transitioned when the render pass is finished

    // As we want to present the color buffer to the swapchain, we transition to PRESENT_KHR
    // Depth attachment
    attachments[1].format = resources::get_texture_format(pipeline.m_depth_stencil);
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at start of first subpass
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;						// We don't need depth after render pass has finished (DONT_CARE may result in better performance)
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// No stencil
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				// No Stencil
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						// Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	// Transition to depth/stencil attachment

    // Setup attachment references
    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;													// Attachment 0 is color
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;				// Attachment layout used as color during the subpass

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;													// Attachment 1 is color
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;		// Attachment used as depth/stemcil used during the subpass

    // Setup a single subpass reference
    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;									// Subpass uses one color attachment
    subpass_description.pColorAttachments = &color_reference;						// Reference to the color attachment in slot 0
    subpass_description.pDepthStencilAttachment = &depth_reference;					// Reference to the depth attachment in slot 1
    subpass_description.inputAttachmentCount = 0;									// Input attachments can be used to sample from contents of a previous subpass
    subpass_description.pInputAttachments = nullptr;								// (Input attachments not used by this example)
    subpass_description.preserveAttachmentCount = 0;								// Preserved attachments can be used to loop (and preserve) attachments through subpasses
    subpass_description.pPreserveAttachments = nullptr;								// (Preserve attachments not used by this example)
    subpass_description.pResolveAttachments = nullptr;								// Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling

    // Setup subpass dependencies
    // These will add the implicit attachment layout transitions specified by the attachment descriptions
    // The actual usage layout is preserved through the layout specified in the attachment reference
    // Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
    // srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
    // Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)
    std::array<VkSubpassDependency, 2> dependencies;

    // First dependency at the start of the renderpass
    // Does the transition from final to initial layout
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;								// Producer of the dependency
    dependencies[0].dstSubpass = 0;													// Consumer is our single subpass that will wait for the execution depdendency
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Second dependency at the end the renderpass
    // Does the transition from the initial to the final layout
    dependencies[1].srcSubpass = 0;													// Producer of the dependency is our single subpass
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;								// Consumer are all commands outside of the renderpass
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());	// Number of attachments used by this render pass
    render_pass_info.pAttachments = attachments.data();								// Descriptions of the attachments used by the render pass
    render_pass_info.subpassCount = 1;												// We only use one subpass in this example
    render_pass_info.pSubpasses = &subpass_description;								// Description of that subpass
    render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());	// Number of subpass dependencies
    render_pass_info.pDependencies = dependencies.data();							// Subpass dependencies used by the render pass

    VK_VERIFY(vkCreateRenderPass(renderer->m_device, &render_pass_info, nullptr, &pipeline.m_render_pass));
    return true;
}

bool create_forward_descriptor_set(c_renderer3d* renderer, s_pipeline& pipeline) {
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = nullptr;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = &layout_binding;

    VK_VERIFY (vkCreateDescriptorSetLayout(renderer->m_device, &descriptor_layout, nullptr, &pipeline.m_descriptor_set_layout));

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &pipeline.m_descriptor_set_layout;

    VK_VERIFY (vkCreatePipelineLayout(renderer->m_device, &pipeline_layout_create_info, nullptr, &pipeline.m_pipeline_layout));
    return true;
}

bool create_forward_pipeline(c_renderer3d* renderer, s_pipeline& pipeline) {

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.pNext = nullptr;
    input_assembly_state.flags = 0;
    input_assembly_state.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state.primitiveRestartEnable = VK_TRUE;


    VkPipelineRasterizationStateCreateInfo rasterization_state = {};
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.pNext = nullptr;
    rasterization_state.flags = 0;
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode =  VkPolygonMode::VK_POLYGON_MODE_FILL;
    rasterization_state.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
    rasterization_state.frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.depthBiasEnable = VK_FALSE;
    rasterization_state.depthBiasConstantFactor = 0.0f;
    rasterization_state.depthBiasClamp = 0.0f;
    rasterization_state.depthBiasSlopeFactor = 0.0f;
    rasterization_state.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
    color_blend_attachment_state.blendEnable = VK_FALSE;
    /*color_blend_attachment_state.srcColorBlendFactor;
    color_blend_attachment_state.dstColorBlendFactor;
    color_blend_attachment_state.colorBlendOp;
    color_blend_attachment_state.srcAlphaBlendFactor;
    color_blend_attachment_state.dstAlphaBlendFactor;
    color_blend_attachment_state.alphaBlendOp;*/
    color_blend_attachment_state.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo color_blend_state = {};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.pNext = nullptr;
    color_blend_state.flags = 0;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment_state;
    color_blend_state.blendConstants[0]
            = color_blend_state.blendConstants[1]
            = color_blend_state.blendConstants[2]
            = color_blend_state.blendConstants[3] = 0.0f;


    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;
    viewport_state.flags = 0;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = nullptr;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = nullptr;


    std::vector<VkDynamicState> dynamic_states;
    dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);

    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pNext = nullptr;
    dynamic_state.flags = 0;
    dynamic_state.dynamicStateCount = (uint32_t) dynamic_states.size();
    dynamic_state.pDynamicStates = dynamic_states.data();


    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.pNext = nullptr;
    depth_stencil_state.flags = 0;
    depth_stencil_state.depthTestEnable = VK_TRUE;
    depth_stencil_state.depthWriteEnable = VK_TRUE;
    depth_stencil_state.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil_state.front = depth_stencil_state.back;
    depth_stencil_state.minDepthBounds = 0.0f;
    depth_stencil_state.maxDepthBounds = 1.0f;


    VkPipelineMultisampleStateCreateInfo multisample_state = {};
    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.pNext = nullptr;
    multisample_state.flags = 0;
    multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state.sampleShadingEnable = VK_FALSE;
    multisample_state.minSampleShading = 0.0f;
    multisample_state.alphaToCoverageEnable = VK_FALSE;
    multisample_state.alphaToOneEnable = VK_FALSE;
    multisample_state.pSampleMask = nullptr;


    resources::shander_handle vertex_shader, fragment_shader;
    VERIFY(resources::load_shader("data/shaders/triangle.vert.spv", resources::e_shader_type::vertex, "main", &vertex_shader));
    VERIFY(resources::load_shader("data/shaders/triangle.frag.spv", resources::e_shader_type::fragment, "main", &fragment_shader));

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
    resources::get_shader_stage_info(vertex_shader, shader_stages[0]);
    resources::get_shader_stage_info(fragment_shader, shader_stages[0]);


    std::vector<e_vertex_component> vertex_components = {
            e_vertex_component::position,
            e_vertex_component::color
    };

    std::vector<std::vector<e_vertex_component>> vertex_desc = {
            vertex_components
    };

    s_vertex_input_desc vertex_input_desc(vertex_desc);


    VkGraphicsPipelineCreateInfo create_info = {
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0,
            (uint32_t) shader_stages.size(), shader_stages.data(),
            &vertex_input_desc.m_vertex_input_state_create_info,
            &input_assembly_state,
            nullptr,
            &viewport_state,
            &rasterization_state,
            &multisample_state,
            &depth_stencil_state,
            &color_blend_state,
            &dynamic_state,
            pipeline.m_pipeline_layout,
            pipeline.m_render_pass, 0,
            VK_NULL_HANDLE, -1
    };

    VK_VERIFY(vkCreateGraphicsPipelines(renderer->m_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline.m_pipeline));
    return true;
}

bool create_forward_framebuffer(c_renderer3d* renderer, s_pipeline& pipeline) {
    pipeline.m_framebuffers.resize(renderer->m_swapchain.images.size());
    
    for (size_t i = 0; i < pipeline.m_framebuffers.size(); i++) {
        std::array<VkImageView, 2> attachments;
        attachments[0] = renderer->m_swapchain.views[i];                               // Color attachment is the view of the swapchain image			
        attachments[1] = resources::get_texture_image_view(pipeline.m_depth_stencil);  // Depth/Stencil attachment is the same for all frame buffers

        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = pipeline.m_render_pass;
        framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_create_info.pAttachments = attachments.data();
        framebuffer_create_info.width = renderer->m_surface.capabilities.currentExtent.width;
        framebuffer_create_info.height = renderer->m_surface.capabilities.currentExtent.height;
        framebuffer_create_info.layers = 1;

        VK_VERIFY(vkCreateFramebuffer(renderer->m_device, &framebuffer_create_info, nullptr, &pipeline.m_framebuffers[i]));
    }

    return true;
}

bool create_forward_rendering_pipeline(c_renderer3d* renderer, s_pipeline& pipeline) {
    VERIFY(create_forward_depth_stencil(renderer, pipeline));
    VERIFY(create_forward_render_pass(renderer, pipeline));
    VERIFY(create_forward_descriptor_set(renderer, pipeline));
    VERIFY(create_forward_pipeline(renderer, pipeline));
    VERIFY(create_forward_framebuffer(renderer, pipeline));
    return true;
}

}
}
}