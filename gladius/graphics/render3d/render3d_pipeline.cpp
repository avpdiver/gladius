//
// Created by alexa on 028 28 08 2016.
//

#include <string>
#include <vector>

#include "../../core/filesystem/filesystem.h"
#include "../../core/filesystem/json_file.h"

#include "render3d_globals.h"
#include "render3d_pipeline.h"
#include "render3d_swapchain.h"
#include "render3d_utils.h"
#include "render3d_texture.h"

namespace gladius { namespace graphics { namespace render3d { namespace resources {

struct s_swapchain_desc {
    std::string format = "R8G8B8A8_UNORM";
    size_t images = 1;

    JSON_FIELDS(
            JSON_FIELD(s_swapchain_desc, format),
            JSON_FIELD(s_swapchain_desc, images)
    );

    bool create() {
        VkFormat vkFormat = utils::string_to_format(format);
        if (vkFormat == VK_FORMAT_UNDEFINED) {
            return false;
        }
        VERIFY(create_swap_chain(vkFormat, images));
        return true;
    }
};

struct s_framebuffer_desc {
    bool swapchain = false;
    float width = 1.0f;
    float height = 1.0f;
    std::string format = "R8G8B8A8_UNORM";
    uint32_t samples = 1;
    uint32_t layer = 1;
    JSON_FIELDS(
            JSON_FIELD(s_framebuffer_desc, swapchain),
            JSON_FIELD(s_framebuffer_desc, width),
            JSON_FIELD(s_framebuffer_desc, height),
            JSON_FIELD(s_framebuffer_desc, format),
            JSON_FIELD(s_framebuffer_desc, samples),
            JSON_FIELD(s_framebuffer_desc, layer)
    );

    static bool create(std::vector<s_framebuffer_info>& buffers, const std::vector<s_framebuffer_desc>& descs) {
        VkFramebufferCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        buffers.resize(vk_globals::swapchain.views.size());

        if (descs.empty()) { // create from swapchain
            info.width = vk_globals::swapchain.width;
            info.height = vk_globals::swapchain.height;
            info.layers = 1;
            info.attachmentCount = 1;
            for (size_t i = 0; i < vk_globals::swapchain.views.size(); ++i) {
                info.pAttachments = &(vk_globals::swapchain.views[i]);
                VK_VERIFY(vkCreateFramebuffer(vk_globals::device, &info, nullptr, &(buffers[i].m_handle)));
            }
        } else {

            for (size_t i = 0; i < descs.size(); ++i) {
                const auto& d = descs[i];
                VkFormat format = utils::string_to_format(d.format);
                VERIFY(format != VK_FORMAT_UNDEFINED);
                uint32_t width = (uint32_t) (vk_globals::swapchain.width * d.width);
                uint32_t height = (uint32_t) (vk_globals::swapchain.height * d.height);
                VkSampleCountFlagBits samples = utils::get_sample_count(d.samples);

                VERIFY(create_texture(format, width, height, 1, 1, 1, samples,
                                      VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                      ));
            }
        }

        return true;
    }

    bool create(size_t index) const {
        VkFramebufferCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.width = (uint32_t) (vk_globals::swapchain.width * width);
        info.height = (uint32_t) (vk_globals::swapchain.height * height);
        info.layers = layer;
        return true;
    }
};

struct s_viewport_desc {
    float x = 0.0;
    float y = 0.0;
    float width = 1.0;
    float height = 1.0;
    float minDepth = 0.0;
    float maxDepth = 1.0;
    JSON_FIELDS(
            JSON_FIELD(s_viewport_desc, x),
            JSON_FIELD(s_viewport_desc, y),
            JSON_FIELD(s_viewport_desc, width),
            JSON_FIELD(s_viewport_desc, height),
            JSON_FIELD(s_viewport_desc, minDepth),
            JSON_FIELD(s_viewport_desc, maxDepth)
    );

	VkViewport&& get() const {
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

struct s_scissor_desc {
    int32_t xOffset = 0;
    int32_t yOffset = 0;
    float width = 1.0f;
    float height = 1.0f;
    JSON_FIELDS(
            JSON_FIELD(s_scissor_desc, xOffset),
            JSON_FIELD(s_scissor_desc, yOffset),
            JSON_FIELD(s_scissor_desc, width),
            JSON_FIELD(s_scissor_desc, height)
    );

	VkRect2D&& get() const {
		VkRect2D info;
        info.offset.x = xOffset;
        info.offset.y = yOffset;
        info.extent.width = (uint32_t) (vk_globals::swapchain.width * width);
        info.extent.height = (uint32_t) (vk_globals::swapchain.height * height);
		return std::move(info);
    }
};

struct s_viewport_state_desc {
    std::vector<s_viewport_desc> viewports;
    std::vector<s_scissor_desc> scissors;

    JSON_FIELDS(
            JSON_FIELD(s_viewport_state_desc, viewports),
            JSON_FIELD(s_viewport_state_desc, scissors)
    );

    void get(s_pipeline_create_info &info) const {
        info.m_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.m_viewport_state.pNext = nullptr;
        info.m_viewport_state.flags = 0;

        info.m_viewport_state.viewportCount = viewports.size();
        info.m_viewport_state.scissorCount = scissors.size();

		for (const auto& v : viewports) {
			info.m_viewports.push_back(v.get());
		}
		info.m_viewport_state.pViewports = &(info.m_viewports[0]);

		for (const auto& s : scissors) {
			info.m_scissors.push_back(s.get());
		}
		info.m_viewport_state.pScissors = &(info.m_scissors[0]);
    }
};

struct s_rasterization_state_desc {
    bool depthClampEnable = false;
    bool rasterizerDiscardEnable = false;
    std::string polygonMode = "fill";
    std::string cullMode = "back";
    std::string frontFace = "counter_clockwise";
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float lineWidth = 1.0f;
    JSON_FIELDS(
            JSON_FIELD(s_rasterization_state_desc, depthClampEnable),
            JSON_FIELD(s_rasterization_state_desc, rasterizerDiscardEnable),
            JSON_FIELD(s_rasterization_state_desc, polygonMode),
            JSON_FIELD(s_rasterization_state_desc, cullMode),
            JSON_FIELD(s_rasterization_state_desc, frontFace),
            JSON_FIELD(s_rasterization_state_desc, depthBiasEnable),
            JSON_FIELD(s_rasterization_state_desc, depthBiasConstantFactor),
            JSON_FIELD(s_rasterization_state_desc, depthBiasClamp),
            JSON_FIELD(s_rasterization_state_desc, depthBiasSlopeFactor),
            JSON_FIELD(s_rasterization_state_desc, lineWidth)
    );

private:
    VkPolygonMode get_polygon_mode() const {
        if (polygonMode == "fill")
            return VK_POLYGON_MODE_FILL;
        if (polygonMode == "line")
            return VK_POLYGON_MODE_LINE;
        if (polygonMode == "point")
            return VK_POLYGON_MODE_POINT;
        return VK_POLYGON_MODE_FILL;
    }

    VkCullModeFlags get_cull_mode() const {
        VkCullModeFlags flag = VK_CULL_MODE_NONE;
        size_t pos = 0;
        size_t n = 0;
        while (n < cullMode.length()) {
            n = cullMode.find_first_of(' ', pos);
            if (n == std::string::npos) {
                n = cullMode.length();
            }
            std::string token = cullMode.substr(pos, n);
            if (token == "front")
                flag |= VK_CULL_MODE_FRONT_BIT;
            if (token == "back")
                flag |= VK_CULL_MODE_BACK_BIT;
        }
        return flag;
    }

    VkFrontFace get_front_face() const {
        if (frontFace == "counter_clockwise")
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        if (frontFace == "clockwise")
            return VK_FRONT_FACE_CLOCKWISE;
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

public:
    void get(VkPipelineRasterizationStateCreateInfo& info) const {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.depthClampEnable = depthClampEnable;
        info.rasterizerDiscardEnable = rasterizerDiscardEnable;
        info.polygonMode = get_polygon_mode();
        info.cullMode = get_cull_mode();
        info.frontFace = get_front_face();
        info.depthBiasEnable = depthBiasEnable;
        info.depthBiasConstantFactor = depthBiasConstantFactor;
        info.depthBiasClamp = depthBiasClamp;
        info.depthBiasSlopeFactor = depthBiasSlopeFactor;
        info.lineWidth = lineWidth;
    }
};

struct s_multisample_state_desc {
    size_t rasterizationSamples = 1;
    bool sampleShadingEnable = false;
    float minSampleShading = 1.0f;
    bool alphaToCoverageEnable = false;
    bool alphaToOneEnable = false;
    JSON_FIELDS(
            JSON_FIELD(s_multisample_state_desc, rasterizationSamples),
            JSON_FIELD(s_multisample_state_desc, sampleShadingEnable),
            JSON_FIELD(s_multisample_state_desc, minSampleShading),
            JSON_FIELD(s_multisample_state_desc, alphaToCoverageEnable),
            JSON_FIELD(s_multisample_state_desc, alphaToOneEnable)
    );
public:
    void get(VkPipelineMultisampleStateCreateInfo & info) const {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.rasterizationSamples = utils::get_sample_count(rasterizationSamples);
        info.sampleShadingEnable = sampleShadingEnable;
        info.minSampleShading = minSampleShading;
        info.alphaToCoverageEnable = alphaToCoverageEnable;
        info.alphaToOneEnable = alphaToOneEnable;
    }
};

struct s_color_blend_attachment_state_desc {
    bool blendEnable = false;
    std::string srcColorBlendFactor = "one";
    std::string dstColorBlendFactor = "zero";
    std::string colorBlendOp = "add";
    std::string srcAlphaBlendFactor = "one";
    std::string dstAlphaBlendFactor = "zero";
    std::string alphaBlendOp = "add";
    std::string colorWriteMask = "rgba";
    JSON_FIELDS(
            JSON_FIELD(s_color_blend_attachment_state_desc, blendEnable),
            JSON_FIELD(s_color_blend_attachment_state_desc, srcColorBlendFactor),
            JSON_FIELD(s_color_blend_attachment_state_desc, dstColorBlendFactor),
            JSON_FIELD(s_color_blend_attachment_state_desc, colorBlendOp),
            JSON_FIELD(s_color_blend_attachment_state_desc, srcAlphaBlendFactor),
            JSON_FIELD(s_color_blend_attachment_state_desc, dstAlphaBlendFactor),
            JSON_FIELD(s_color_blend_attachment_state_desc, alphaBlendOp),
            JSON_FIELD(s_color_blend_attachment_state_desc, colorWriteMask)
    );

private:
    VkBlendFactor get_blend_color(const std::string& value) const {
        if (value == "zero")
            return VK_BLEND_FACTOR_ZERO;
        if (value == "one")
            return VK_BLEND_FACTOR_ONE;
        if (value == "src_color")
            return VK_BLEND_FACTOR_SRC_COLOR;
        if (value == "one_minus_src_color")
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        if (value == "dst_color")
            return VK_BLEND_FACTOR_DST_COLOR;
        if (value == "one_minus_dst_color")
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        if (value == "src_alpha")
            return VK_BLEND_FACTOR_SRC_ALPHA;
        if (value == "one_minus_src_alpha")
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        if (value == "dst_alpha")
            return VK_BLEND_FACTOR_DST_ALPHA;
        if (value == "one_minus_dst_alpha")
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        if (value == "constant_color")
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        if (value == "one_minus_constant_color")
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        if (value == "constant_alpha")
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        if (value == "one_minus_constant_alpha")
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        if (value == "src_alpha_saturate")
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        if (value == "src1_color")
            return VK_BLEND_FACTOR_SRC1_COLOR;
        if (value == "one_minus_src1_color")
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        if (value == "src1_alpha")
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        if (value == "one_minus_src1_alpha")
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        return VK_BLEND_FACTOR_ZERO;
    }

    VkBlendOp get_blend_op(const std::string& value) const {
        if (value == "add")
            return VK_BLEND_OP_ADD;
        if (value == "subtract")
            return VK_BLEND_OP_SUBTRACT;
        if (value == "reverse_subtract")
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        if (value == "min")
            return VK_BLEND_OP_MIN;
        if (value == "max")
            return VK_BLEND_OP_MAX;
        return VK_BLEND_OP_ADD;
    }

    VkColorComponentFlags get_color_comp_flag() const {
        VkColorComponentFlags flag = 0;
        if (colorWriteMask.find('r') != std::string::npos)
            flag |= VK_COLOR_COMPONENT_R_BIT;
        if (colorWriteMask.find('g') != std::string::npos)
            flag |= VK_COLOR_COMPONENT_G_BIT;
        if (colorWriteMask.find('b') != std::string::npos)
            flag |= VK_COLOR_COMPONENT_B_BIT;
        if (colorWriteMask.find('a') != std::string::npos)
            flag |= VK_COLOR_COMPONENT_A_BIT;
        return flag;
    }

public:
	VkPipelineColorBlendAttachmentState&& get() const {
		VkPipelineColorBlendAttachmentState info;
        info.blendEnable = blendEnable;
        info.srcColorBlendFactor = get_blend_color(srcColorBlendFactor);
        info.dstColorBlendFactor = get_blend_color(dstColorBlendFactor);
        info.colorBlendOp = get_blend_op(colorBlendOp);
        info.srcAlphaBlendFactor = get_blend_color(srcAlphaBlendFactor);
        info.dstAlphaBlendFactor = get_blend_color(dstAlphaBlendFactor);
        info.alphaBlendOp = get_blend_op(alphaBlendOp);
        info.colorWriteMask = get_color_comp_flag();
		return std::move(info);
    }
};

struct s_color_blend_state_desc {
    bool logicOpEnable = false;
    std::string logicOp = "copy";
    std::vector<float> blendConstants{0.0, 0.0, 0.0, 0.0};
    std::vector<s_color_blend_attachment_state_desc> color_blend_attachment_states;

    JSON_FIELDS(
            JSON_FIELD(s_color_blend_state_desc, logicOpEnable),
            JSON_FIELD(s_color_blend_state_desc, logicOp),
            JSON_FIELD(s_color_blend_state_desc, blendConstants),
            JSON_FIELD(s_color_blend_state_desc, color_blend_attachment_states)
    );

private:
    VkLogicOp get_logic_op() const {
        if (logicOp == "clear" )
            return VK_LOGIC_OP_CLEAR;
        if (logicOp == "and" )
            return VK_LOGIC_OP_AND;
        if (logicOp == "and_reverse" )
            return VK_LOGIC_OP_AND_REVERSE;
        if (logicOp == "copy" )
            return VK_LOGIC_OP_COPY;
        if (logicOp == "and_inverted" )
            return VK_LOGIC_OP_AND_INVERTED;
        if (logicOp == "no_op" )
            return VK_LOGIC_OP_NO_OP;
        if (logicOp == "xor" )
            return VK_LOGIC_OP_XOR;
        if (logicOp == "or" )
            return VK_LOGIC_OP_OR;
        if (logicOp == "nor" )
            return VK_LOGIC_OP_NOR;
        if (logicOp == "equivalent" )
            return VK_LOGIC_OP_EQUIVALENT;
        if (logicOp == "invert" )
            return VK_LOGIC_OP_INVERT;
        if (logicOp == "or_reverse" )
            return VK_LOGIC_OP_OR_REVERSE;
        if (logicOp == "copy_inverted" )
            return VK_LOGIC_OP_COPY_INVERTED;
        if (logicOp == "or_inverted" )
            return VK_LOGIC_OP_OR_INVERTED;
        if (logicOp == "nand" )
            return VK_LOGIC_OP_NAND;
        if (logicOp == "set")
            return VK_LOGIC_OP_SET;
        return VK_LOGIC_OP_COPY;
    }

public:
    void get(s_pipeline_create_info& info) {
        info.m_color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.m_color_blend_state.pNext = nullptr;
        info.m_color_blend_state.flags = 0;

        info.m_color_blend_state.logicOpEnable = logicOpEnable;
        info.m_color_blend_state.logicOp = get_logic_op();
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
            for (auto& f : blendConstants) {
                info.m_color_blend_state.blendConstants[i++] = f;
            }
        }

		info.m_color_blend_state.attachmentCount = color_blend_attachment_states.size();
		for (const auto& a : color_blend_attachment_states) {
			info.m_color_blend_attachment_states.push_back(a.get());
		}
		info.m_color_blend_state.pAttachments = &(info.m_color_blend_attachment_states[0]);

    }
};

struct s_pipeline_desc {
	std::string name;
    s_swapchain_desc swapchain;
	std::vector<s_framebuffer_desc> framebuffers;
	s_viewport_state_desc viewport_state;
	s_rasterization_state_desc rasterization_state;
	s_multisample_state_desc multisample_state;
	s_color_blend_state_desc color_blend_state;
	JSON_FIELDS(
		JSON_FIELD(s_pipeline_desc, name),
        JSON_FIELD(s_pipeline_desc, swapchain),
		JSON_FIELD(s_pipeline_desc, framebuffers),
		JSON_FIELD(s_pipeline_desc, viewport_state),
		JSON_FIELD(s_pipeline_desc, rasterization_state),
		JSON_FIELD(s_pipeline_desc, multisample_state),
		JSON_FIELD(s_pipeline_desc, color_blend_state)
	);

	bool create(s_pipeline_create_info &info) {
        VERIFY(swapchain.create());
        VERIFY(s_framebuffer_desc::create(info.m_framebuffers, framebuffers));

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

s_pipeline_desc g_pipeline_desc;

bool load_pipeline(const char *filename) {
    core::filesystem::c_json_file* file = reinterpret_cast<core::filesystem::c_json_file*>(
        core::filesystem::open("disk:json", filename, core::filesystem::e_file_mode::read));

    file->read(g_pipeline_desc);
	core::filesystem::close(file);
    return true;
}

bool create_pipeline() {
    s_pipeline_create_info pipeline_create_info;
    return g_pipeline_desc.create(pipeline_create_info);
}

}}}}