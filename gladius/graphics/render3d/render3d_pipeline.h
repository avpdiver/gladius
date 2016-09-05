//
// Created by alexa on 028 28 08 2016.
//

#ifndef GLADIUS_RENDER3D_PIPELINE_H
#define GLADIUS_RENDER3D_PIPELINE_H


namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_pipeline_create_info {
	s_pipeline_create_info();

	VkPipelineShaderStageCreateInfo m_stages;
	VkPipelineVertexInputStateCreateInfo m_vertex_input_state;
	VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state;
	VkPipelineTessellationStateCreateInfo m_tessellation_state;
	VkPipelineViewportStateCreateInfo m_viewport_state;
	VkPipelineRasterizationStateCreateInfo m_rasterization_state;
	VkPipelineMultisampleStateCreateInfo m_multisample_state;
	VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state;
	VkPipelineColorBlendStateCreateInfo m_color_blend_state;
	VkPipelineDynamicStateCreateInfo m_dynamic_state;
	VkGraphicsPipelineCreateInfo m_pipeline_create_info;

	std::vector<VkViewport> m_viewports;
	std::vector<VkRect2D> m_scissors;
	std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachment_states;
};

bool load_pipeline(const char *filename);

}
}
}
}

#endif //GLADIUS_RENDER3D_PIPELINE_H
