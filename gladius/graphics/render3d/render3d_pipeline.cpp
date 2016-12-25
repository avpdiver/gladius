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

static constexpr auto float_width_func = get_swapchain_width<float, float>;
static constexpr auto float_height_func = get_swapchain_height<float, float>;

static constexpr auto uint32_width_func = get_swapchain_width<uint32_t, float>;
static constexpr auto uint32_height_func = get_swapchain_height<uint32_t, float>;

VkBool32 bool_to_uint32(const bool& v) {
    return v == true ? 1 : 0;
}

struct s_swapchain_json {
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    size_t images = 1;

    JSON_FIELDS(
            JSON_FIELD_CONV(s_swapchain_json, format, utils::string_to_format),
            JSON_FIELD(s_swapchain_json, images)
    );

    bool create() {
        if (format == VK_FORMAT_UNDEFINED) {
            return false;
        }
        VERIFY(create_swap_chain(format, images));
        return true;
    }
};

struct s_viewport_json : public VkViewport {
    JSON_FIELDS(
            JSON_FIELD(s_viewport_json, x),
            JSON_FIELD(s_viewport_json, y),
            JSON_FIELD_CONV(s_viewport_json, width, float_width_func),
            JSON_FIELD_CONV(s_viewport_json, height, float_height_func),
            JSON_FIELD(s_viewport_json, minDepth),
            JSON_FIELD(s_viewport_json, maxDepth)
    );
};

struct s_scissor_json {
    int32_t xOffset = 0;
    int32_t yOffset = 0;
    float width = 1.0f;
    float height = 1.0f;

    JSON_FIELDS(
            JSON_FIELD(s_scissor_json, xOffset),
            JSON_FIELD(s_scissor_json, yOffset),
            JSON_FIELD_CONV(s_scissor_json, width, float_width_func),
            JSON_FIELD_CONV(s_scissor_json, height, float_height_func)
    );

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

    JSON_FIELDS(
            JSON_FIELD(s_viewport_state_json, viewports),
            JSON_FIELD(s_viewport_state_json, scissors)
    );

    VkPipelineViewportStateCreateInfo&& get(s_pipeline_create_info &pipeline) const {
        VkPipelineViewportStateCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.viewportCount = viewports.size();
        info.scissorCount = scissors.size();

        for (const auto &v : viewports) {
            pipeline.m_viewports.push_back(v);
        }
        info.pViewports = &(pipeline.m_viewports[0]);

        for (const auto &s : scissors) {
            pipeline.m_scissors.push_back(s.get());
        }
        info.pScissors = &(pipeline.m_scissors[0]);

        return std::move(info);
    }
};

struct s_rasterization_state_json : public VkPipelineRasterizationStateCreateInfo {
    JSON_FIELDS(
            JSON_FIELD_CONV(s_rasterization_state_json, depthClampEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_rasterization_state_json, rasterizerDiscardEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_rasterization_state_json, polygonMode, utils::string_to_polygon_mode),
            JSON_FIELD_CONV(s_rasterization_state_json, cullMode, utils::string_to_cull_mode),
            JSON_FIELD_CONV(s_rasterization_state_json, frontFace, utils::string_to_front_face),
            JSON_FIELD_CONV(s_rasterization_state_json, depthBiasEnable, bool_to_uint32),
            JSON_FIELD(s_rasterization_state_json, depthBiasConstantFactor),
            JSON_FIELD(s_rasterization_state_json, depthBiasClamp),
            JSON_FIELD(s_rasterization_state_json, depthBiasSlopeFactor),
            JSON_FIELD(s_rasterization_state_json, lineWidth)
    );

public:
    s_rasterization_state_json() {
        sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pNext = nullptr;
        flags = 0;
    }
};

struct s_multisample_state_json : public VkPipelineMultisampleStateCreateInfo {
    JSON_FIELDS(
            JSON_FIELD_CONV(s_multisample_state_json, rasterizationSamples, utils::get_sample_count),
            JSON_FIELD_CONV(s_multisample_state_json, sampleShadingEnable, bool_to_uint32),
            JSON_FIELD(s_multisample_state_json, minSampleShading),
            JSON_FIELD_CONV(s_multisample_state_json, alphaToCoverageEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_multisample_state_json, alphaToOneEnable, bool_to_uint32)
    );

public:
    s_multisample_state_json() {
        sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pNext = nullptr;
        flags = 0;
    }
};

struct s_color_blend_attachment_state_json : public VkPipelineColorBlendAttachmentState {
    JSON_FIELDS(
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, blendEnable, bool_to_uint32),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, srcColorBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, dstColorBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, colorBlendOp, utils::string_to_blend_op),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, srcAlphaBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, dstAlphaBlendFactor, utils::string_to_blend_color),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, alphaBlendOp, utils::string_to_blend_op),
            JSON_FIELD_CONV(s_color_blend_attachment_state_json, colorWriteMask, utils::string_to_color_component_flag)
    );
};

struct s_color_blend_state_json {
    bool logicOpEnable = false;
    VkLogicOp logicOp = VK_LOGIC_OP_COPY;
    std::vector<float> blendConstants {0.0, 0.0, 0.0, 0.0};
    std::vector<s_color_blend_attachment_state_json> color_blend_attachment_states;

    JSON_FIELDS(
            JSON_FIELD(s_color_blend_state_json, logicOpEnable),
            JSON_FIELD_CONV(s_color_blend_state_json, logicOp, utils::string_to_logic_op),
            JSON_FIELD(s_color_blend_state_json, blendConstants),
            JSON_FIELD(s_color_blend_state_json, color_blend_attachment_states)
    );

public:
    VkPipelineColorBlendStateCreateInfo&& get(s_pipeline_create_info &pipeline) const {
        VkPipelineColorBlendStateCreateInfo info;

        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.logicOpEnable = logicOpEnable;
        info.logicOp = logicOp;
        if (blendConstants.size() == 0) {
            info.blendConstants[0]
                    = info.blendConstants[1]
                    = info.blendConstants[2]
                    = info.blendConstants[3] = 0.0f;
        } else if (blendConstants.size() == 1) {
            info.blendConstants[0]
                    = info.blendConstants[1]
                    = info.blendConstants[2]
                    = info.blendConstants[3] = blendConstants[0];
        } else {
            size_t i = 0;
            for (auto &f : blendConstants) {
                info.blendConstants[i++] = f;
            }
        }

        info.attachmentCount = color_blend_attachment_states.size();
        for (const auto &a : color_blend_attachment_states) {
            pipeline.m_color_blend_attachment_states.push_back(a);
        }
        info.pAttachments = &(pipeline.m_color_blend_attachment_states[0]);

    }
};

struct s_renderpass_attachment_json : public VkAttachmentDescription {
    JSON_FIELDS(
            JSON_FIELD(s_renderpass_attachment_json, flags),
            JSON_FIELD_CONV(s_renderpass_attachment_json, format, utils::string_to_format),
            JSON_FIELD_CONV(s_renderpass_attachment_json, samples, utils::get_sample_count),
            JSON_FIELD_CONV(s_renderpass_attachment_json, loadOp, utils::string_to_load_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, storeOp, utils::string_to_store_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, stencilLoadOp, utils::string_to_load_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, stencilStoreOp, utils::string_to_store_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, initialLayout, utils::string_to_image_layout),
            JSON_FIELD_CONV(s_renderpass_attachment_json, finalLayout, utils::string_to_image_layout)
    );
};

struct s_attachment_ref_json : public VkAttachmentReference {
    JSON_FIELDS(
            JSON_FIELD(s_attachment_ref_json, attachment),
            JSON_FIELD_CONV(s_attachment_ref_json, layout, utils::string_to_image_layout)
    );
};

struct s_subpass_json {
    VkSubpassDescriptionFlags flags;
    VkPipelineBindPoint pipelineBindPoint;
    std::vector<s_attachment_ref_json> inputAttachments;
    std::vector<s_attachment_ref_json> colorAttachments;
    std::vector<s_attachment_ref_json> resolveAttachments;
    std::vector<s_attachment_ref_json> depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;

    JSON_FIELDS(
            JSON_FIELD(s_subpass_json, flags),
            JSON_FIELD_CONV(s_subpass_json, pipelineBindPoint, utils::string_to_bind_point),
            JSON_FIELD(s_subpass_json, inputAttachments),
            JSON_FIELD(s_subpass_json, colorAttachments),
            JSON_FIELD(s_subpass_json, resolveAttachments),
            JSON_FIELD(s_subpass_json, depthStencilAttachment),
            JSON_FIELD(s_subpass_json, preserveAttachments)
    );
};

struct s_renderpass_json {
    std::vector<s_renderpass_attachment_json> attachments;
    std::vector<s_subpass_json> subpasses;

    JSON_FIELDS(
            JSON_FIELD(s_renderpass_json, attachments),
            JSON_FIELD(s_renderpass_json, subpasses)
    );
};

struct s_framebuffer_attachment_json {
    bool swapchain = false;
    uint32_t width;
    uint32_t height;
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    uint32_t layer = 1;

    JSON_FIELDS(
            JSON_FIELD(s_framebuffer_attachment_json, swapchain),
            JSON_FIELD_CONV(s_framebuffer_attachment_json, width, uint32_width_func),
            JSON_FIELD_CONV(s_framebuffer_attachment_json, height, uint32_height_func),
            JSON_FIELD_CONV(s_framebuffer_attachment_json, format, utils::string_to_format),
            JSON_FIELD_CONV(s_framebuffer_attachment_json, samples, utils::get_sample_count),
            JSON_FIELD(s_framebuffer_attachment_json, layer)
    );

    bool create(s_texture_desc **desc) const {
        if (swapchain) {
            *desc = nullptr;
        } else {
            VERIFY(format != VK_FORMAT_UNDEFINED);
            handle_t handle;
            VERIFY(create_texture(format, width, height, 1, 1, 1, samples,
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

    JSON_FIELDS(
            JSON_FIELD(s_framebuffer_json, renderpass),
            JSON_FIELD(s_framebuffer_json, attachments)
    );

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

        if (swapchain_index != static_cast<size_t>(-1)) {
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

    JSON_FIELDS(
            JSON_FIELD(s_pipeline_json, name),
            JSON_FIELD(s_pipeline_json, swapchain),
            JSON_FIELD(s_pipeline_json, viewport_state),
            JSON_FIELD(s_pipeline_json, rasterization_state),
            JSON_FIELD(s_pipeline_json, multisample_state),
            JSON_FIELD(s_pipeline_json, color_blend_state),
            JSON_FIELD(s_pipeline_json, renderpasses),
            JSON_FIELD(s_pipeline_json, framebuffers)
    );

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
    core::filesystem::c_json_file *file = reinterpret_cast<core::filesystem::c_json_file *>(
            core::filesystem::open("disk:json", filename, core::filesystem::e_file_mode::read));

    file->read(g_pipeline_json);
    core::filesystem::close(file);
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