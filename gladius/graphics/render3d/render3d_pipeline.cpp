//
// Created by alexa on 028 28 08 2016.
//

#include <string>
#include <vector>

#include "../../core/filesystem/filesystem.h"

#include "render3d_globals.h"
#include "render3d_pipeline.h"
#include "render3d_swapchain.h"
#include "render3d_utils.h"
#include "render3d_texture.h"
#include "render3d_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

template<typename OUT_TYPE, typename IN_TYPE>
OUT_TYPE get_swapchain_width(const IN_TYPE& v) {
	return static_cast<OUT_TYPE>(vk_globals::swapchain.width * v);
}

template<typename OUT_TYPE, typename IN_TYPE>
OUT_TYPE get_swapchain_height(const IN_TYPE& v) {
	return static_cast<OUT_TYPE>(vk_globals::swapchain.height * v);
}

VkBool32 bool_to_uint32(const bool& v) {
	return v == true ? 1 : 0;
}

struct s_swapchain_json {
	VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
	size_t images = 1;

	bool create() {
		if (format == VK_FORMAT_UNDEFINED) {
			return false;
		}
		VERIFY(create_swap_chain(format, images));
		return true;
	}
};

struct s_viewport_json : public VkViewport {
	static constexpr auto width_func = get_swapchain_width<float, float>;
	static constexpr auto height_func = get_swapchain_height<float, float>;

	VkViewport &&get() const {
		VkViewport info;
		info.x = x;
		info.y = y;
		info.width = vk_globals::swapchain.width * width;
		info.height = vk_globals::swapchain.height * height;
		info.minDepth = minDepth;
		info.maxDepth = maxDepth;
		return std::move(info);
	}
};

struct s_scissor_json {
	static constexpr auto width_func = get_swapchain_width<float, float>;
	static constexpr auto height_func = get_swapchain_height<float, float>;

	int32_t xOffset = 0;
	int32_t yOffset = 0;
	float width = 1.0f;
	float height = 1.0f;

	VkRect2D &&get() const {
		VkRect2D info;
		info.offset.x = xOffset;
		info.offset.y = yOffset;
		info.extent.width = (uint32_t) (vk_globals::swapchain.width * width);
		info.extent.height = (uint32_t) (vk_globals::swapchain.height * height);
		return std::move(info);
	}
};

struct s_viewport_state_json {
	std::vector<s_viewport_json> viewports;
	std::vector<s_scissor_json> scissors;

	void get(s_pipeline_create_info &info) const {
		info.m_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		info.m_viewport_state.pNext = nullptr;
		info.m_viewport_state.flags = 0;

		info.m_viewport_state.viewportCount = viewports.size();
		info.m_viewport_state.scissorCount = scissors.size();

		for (const auto &v : viewports) {
			info.m_viewports.push_back(v.get());
		}
		info.m_viewport_state.pViewports = &(info.m_viewports[0]);

		for (const auto &s : scissors) {
			info.m_scissors.push_back(s.get());
		}
		info.m_viewport_state.pScissors = &(info.m_scissors[0]);
	}
};

struct s_rasterization_state_json : public VkPipelineRasterizationStateCreateInfo {
public:
	void get(VkPipelineRasterizationStateCreateInfo &info) const {
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
	}
};

struct s_multisample_state_json : public VkPipelineMultisampleStateCreateInfo {
public:
	s_multisample_state_json() {
		sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pNext = nullptr;
		flags = 0;
	}
};

struct s_color_blend_attachment_state_json : public VkPipelineColorBlendAttachmentState {
};

struct s_color_blend_state_json {
	bool logicOpEnable = false;
	VkLogicOp logicOp = VK_LOGIC_OP_COPY;
	std::vector<float> blendConstants {0.0, 0.0, 0.0, 0.0};
	std::vector<s_color_blend_attachment_state_json> color_blend_attachment_states;

public:
	void get(s_pipeline_create_info &info) {
		info.m_color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		info.m_color_blend_state.pNext = nullptr;
		info.m_color_blend_state.flags = 0;

		info.m_color_blend_state.logicOpEnable = logicOpEnable;
		info.m_color_blend_state.logicOp = logicOp;
		if (blendConstants.size() == 0) {
			info.m_color_blend_state.blendConstants[0]
				= info.m_color_blend_state.blendConstants[1]
				= info.m_color_blend_state.blendConstants[2]
				= info.m_color_blend_state.blendConstants[3] = 0.0f;
		} else if (blendConstants.size() == 1) {
			info.m_color_blend_state.blendConstants[0]
				= info.m_color_blend_state.blendConstants[1]
				= info.m_color_blend_state.blendConstants[2]
				= info.m_color_blend_state.blendConstants[3] = blendConstants[0];
		} else {
			size_t i = 0;
			for (auto &f : blendConstants) {
				info.m_color_blend_state.blendConstants[i++] = f;
			}
		}

		info.m_color_blend_state.attachmentCount = color_blend_attachment_states.size();
		for (const auto &a : color_blend_attachment_states) {
			info.m_color_blend_attachment_states.push_back(a);
		}
		info.m_color_blend_state.pAttachments = &(info.m_color_blend_attachment_states[0]);

	}
};

struct s_renderpass_attachment_json : public VkAttachmentDescription {
};

struct s_attachment_ref_json : public VkAttachmentReference {
};

struct s_subpass_json {
	VkSubpassDescriptionFlags flags;
	VkPipelineBindPoint pipelineBindPoint;
	std::vector<s_attachment_ref_json> inputAttachments;
	std::vector<s_attachment_ref_json> colorAttachments;
	std::vector<s_attachment_ref_json> resolveAttachments;
	std::vector<s_attachment_ref_json> depthStencilAttachment;
	std::vector<uint32_t> preserveAttachments;
};

struct s_renderpass_json {
	std::vector<s_renderpass_attachment_json> attachments;
	std::vector<s_subpass_json> subpasses;
};

struct s_framebuffer_attachment_json {
	static constexpr auto width_func = get_swapchain_width<uint32_t, float>;
	static constexpr auto height_func = get_swapchain_height<uint32_t, float>;

	bool swapchain = false;
	uint32_t width;
	uint32_t height;
	VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
	VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
	uint32_t layer = 1;

	bool create(s_texture_desc **desc) const {
		if (swapchain) {
			*desc = nullptr;
		} else {
			VERIFY(format != VK_FORMAT_UNDEFINED);
			uint32_t w = (uint32_t) (vk_globals::swapchain.width * width);
			uint32_t h = (uint32_t) (vk_globals::swapchain.height * height);

			handle_t handle;
			VERIFY(create_texture(format, w, h, 1, 1, 1, samples,
								  VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
								  handle));
			*desc = reinterpret_cast<s_texture_desc*>(handle);
		}
		return true;
	}

};

struct s_framebuffer_json {
	size_t renderpass;
	std::vector<s_framebuffer_attachment_json> attachments;

	bool create(s_framebuffer_desc &desc) const {
		VERIFY_LOG(!attachments.empty(), LOG_TYPE, "There is no framebuffers attachments", "");

		bool sc = false;
		for (const auto &a : attachments) {
			if (a.swapchain && sc) {
				VERIFY_LOG(false, LOG_TYPE, "Swapchain attachment appears more then once", "");
			}
		}

		desc.m_attachments.resize(attachments.size());

		VkFramebufferCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.width = 0;
		info.height = 0;

		std::vector<VkImageView> images(attachments.size());
		size_t swapchain_index = -1;
		for (size_t i = 0; i < attachments.size(); ++i) {
			auto &a = desc.m_attachments[i];
			VERIFY(attachments[i].create(&a));
			if (a == nullptr) {
				swapchain_index = i;
				info.width = std::max(info.width, vk_globals::swapchain.width);
				info.height = std::max(info.height, vk_globals::swapchain.height);
			} else {
				images[i] = a->view;
				info.width = std::max(info.width, a->width);
				info.height = std::max(info.height, a->height);
			}
		}

		if (swapchain_index != -1) {
			desc.m_framebuffers.resize(vk_globals::swapchain.images.size());
			for (size_t i = 0; i < vk_globals::swapchain.images.size(); ++i) {
				images[swapchain_index] = vk_globals::swapchain.views[i];
				VK_VERIFY(vkCreateFramebuffer(vk_globals::device, &info, nullptr, &(desc.m_framebuffers[i])));
			}
		} else {
			desc.m_framebuffers.resize(1);
			VK_VERIFY(vkCreateFramebuffer(vk_globals::device, &info, nullptr, &(desc.m_framebuffers[0])));
		}
		return true;
	}
};

struct s_pipeline_json {
	std::string name;
	s_swapchain_json swapchain;
	s_viewport_state_json viewport_state;
	s_rasterization_state_json rasterization_state;
	s_multisample_state_json multisample_state;
	s_color_blend_state_json color_blend_state;
	std::vector<s_renderpass_json> renderpasses;
	std::vector<s_framebuffer_json> framebuffers;

	bool create(s_pipeline_create_info &info) {
		VERIFY_LOG(!framebuffers.empty(), LOG_TYPE, "There is no framebuffers", "");
		VERIFY(swapchain.create());

		info.m_framebuffers.resize(framebuffers.size());
		for (size_t i = 0; i < framebuffers.size(); ++i) {
			VERIFY(framebuffers[i].create(info.m_framebuffers[i]));
		}

		/*
		viewport_state.get(info);
		rasterization_state.get(info.m_rasterization_state);
		multisample_state.get(info.m_multisample_state);
		color_blend_state.get(info);
		*/
		return true;
	}
};

s_pipeline_create_info::s_pipeline_create_info() {
	m_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_pipeline_create_info.pNext = nullptr;
	m_pipeline_create_info.flags = 0;
	m_pipeline_create_info.subpass = 0;
	m_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	m_pipeline_create_info.basePipelineIndex = -1;
	m_pipeline_create_info.pStages = &m_stages;
	m_pipeline_create_info.pVertexInputState = &m_vertex_input_state;
	m_pipeline_create_info.pInputAssemblyState = &m_input_assembly_state;
	m_pipeline_create_info.pTessellationState = &m_tessellation_state;
	m_pipeline_create_info.pViewportState = &m_viewport_state;
	m_pipeline_create_info.pRasterizationState = &m_rasterization_state;
	m_pipeline_create_info.pMultisampleState = &m_multisample_state;
	m_pipeline_create_info.pDepthStencilState = &m_depth_stencil_state;
	m_pipeline_create_info.pColorBlendState = &m_color_blend_state;
	m_pipeline_create_info.pDynamicState = &m_dynamic_state;
}

s_pipeline_json g_pipeline_json;

bool load_pipeline(const char *filename) {
	return true;
}

bool create_pipeline() {
	s_pipeline_create_info pipeline_create_info;
	return g_pipeline_json.create(pipeline_create_info);
}

}
}
}
}