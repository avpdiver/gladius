//
// Created by alexa on 028 28 08 2016.
//

#include <string>
#include <vector>

#include "../../core/filesystem/filesystem.h"
#include "../../core/filesystem/json_file.h"

#include "render3d_globals.h"
#include "render3d_pipeline.h"

namespace gladius { namespace graphics { namespace render3d { namespace resources {

struct s_framebuffer_desc {
    float width = 1.0f;
    float height = 1.0f;
    std::string format = "r8g8b8a8";
    uint32_t samples = 1;
    uint32_t layer = 1;
    JSON_FIELDS(
            JSON_FIELD(s_framebuffer_desc, width),
            JSON_FIELD(s_framebuffer_desc, height),
            JSON_FIELD(s_framebuffer_desc, format),
            JSON_FIELD(s_framebuffer_desc, samples),
            JSON_FIELD(s_framebuffer_desc, layer)
    );

    void get(VkFramebufferCreateInfo& info) {
        info.width = (uint32_t) (vk_globals::swapchain.width * width);
        info.height = (uint32_t) (vk_globals::swapchain.height * height);
        info.layers = layer;

        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
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

    void get(VkViewport& info) {
        info.x = x;
        info.y = y;
        info.width = vk_globals::swapchain.width * width;
        info.height = vk_globals::swapchain.height * height;
        info.minDepth = minDepth;
        info.maxDepth = maxDepth;
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

    void get(VkRect2D& info) {
        info.offset.x = xOffset;
        info.offset.y = yOffset;
        info.extent.width = (uint32_t) (vk_globals::swapchain.width * width);
        info.extent.height = (uint32_t) (vk_globals::swapchain.height * height);
    }
};

struct s_viewport_state_desc {
    std::vector<s_viewport_desc> viewports;
    std::vector<s_scissor_desc> scissors;

    JSON_FIELDS(
            JSON_FIELD(s_viewport_state_desc, viewports),
            JSON_FIELD(s_viewport_state_desc, scissors)
    );

    void get(VkPipelineViewportStateCreateInfo &info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.viewportCount = viewports.size();
        info.scissorCount = scissors.size();
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
    void get(VkPipelineRasterizationStateCreateInfo& info) {
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
private:
    VkSampleCountFlagBits get_samples () {
        if (rasterizationSamples == 1)
            return VK_SAMPLE_COUNT_1_BIT;
        if (rasterizationSamples == 2)
            return VK_SAMPLE_COUNT_2_BIT;
        if (rasterizationSamples == 4)
            return VK_SAMPLE_COUNT_4_BIT;
        if (rasterizationSamples == 8)
            return VK_SAMPLE_COUNT_8_BIT;
        if (rasterizationSamples == 16)
            return VK_SAMPLE_COUNT_16_BIT;
        if (rasterizationSamples == 32)
            return VK_SAMPLE_COUNT_32_BIT;
        if (rasterizationSamples == 64)
            return VK_SAMPLE_COUNT_64_BIT;
        return VK_SAMPLE_COUNT_1_BIT;
    }
public:
    void get(VkPipelineMultisampleStateCreateInfo & info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.rasterizationSamples = get_samples();
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

    VkBlendOp get_blend_op(const std::string& value) {
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

    VkColorComponentFlags get_color_comp_flag() {
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
    void get(VkPipelineColorBlendAttachmentState& info) {
        info.blendEnable = blendEnable;
        info.srcColorBlendFactor = get_blend_color(srcColorBlendFactor);
        info.dstColorBlendFactor = get_blend_color(dstColorBlendFactor);
        info.colorBlendOp = get_blend_op(colorBlendOp);
        info.srcAlphaBlendFactor = get_blend_color(srcAlphaBlendFactor);
        info.dstAlphaBlendFactor = get_blend_color(dstAlphaBlendFactor);
        info.alphaBlendOp = get_blend_op(alphaBlendOp);
        info.colorWriteMask = get_color_comp_flag();
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
    void get(VkPipelineColorBlendStateCreateInfo& info) {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.logicOpEnable = logicOpEnable;
        info.logicOp = get_logic_op();
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
            for (auto& f : blendConstants) {
                info.blendConstants[i++] = f;
            }
        }
    }
};

struct s_pipeline_desc {
    std::string name;
    std::vector<s_framebuffer_desc> framebuffers;
    s_viewport_state_desc viewport;
    s_rasterization_state_desc rasterization_state;
    s_multisample_state_desc multisample_state;
    s_color_blend_attachment_state_desc color_blend_attachment_state;
    JSON_FIELDS(
            JSON_FIELD(s_pipeline_desc, name),
            JSON_FIELD(s_pipeline_desc, framebuffers),
            JSON_FIELD(s_pipeline_desc, viewport),
            JSON_FIELD(s_pipeline_desc, rasterization_state),
            JSON_FIELD(s_pipeline_desc, multisample_state),
            JSON_FIELD(s_pipeline_desc, color_blend_attachment_state)
    );

    void get(VkGraphicsPipelineCreateInfo &info) {
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        /*
        info.pViewportState
        info.pRasterizationState
        info.pMultisampleState
        info.pDepthStencilState
        info.pColorBlendState
        info.subpass = 0;
        info.basePipelineHandle = VK_NULL_HANDLE;
        info.basePipelineIndex = -1;
         */
    }
};

}}}}