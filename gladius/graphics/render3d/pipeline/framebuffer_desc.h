//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_FRAMEBUFFER_DESC_H_H
#define GLADIUS_FRAMEBUFFER_DESC_H_H

#include <vulkan/vulkan.h>
#include "../../../core/meta/json.h"
#include "pipeline_utils.h"
#include "../render3d_utils.h"
#include "../render3d_resources.h"
#include "../render3d_texture.h"
#include "../render3d_macros.h"
#include "../render3d_swapchain.h"
#include "../render3d_framebuffer.h"

namespace gladius {
namespace graphics {
namespace render3d {


struct s_framebuffer_attachment_json {
public:
    ssize_t swapchain;
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

public:
    resources::texture_handle create() const {
        assert(swapchain < 0);

        VERIFY_ASSERT(format != VK_FORMAT_UNDEFINED, LOG_TYPE, "Undefined framebuffer attachment format %i", format);
        resources::texture_handle handle;
        VERIFY_ASSERT(resources::create_texture(format, width, height, 1, 1, 1, samples,
                                                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                &handle),
                      LOG_TYPE, "Failed create framebuffer attachment texture", "");
        return handle;
    }
};

struct s_framebuffer_json {
public:
    size_t renderpass;
    std::vector<s_framebuffer_attachment_json> attachments;

    JSON_FIELDS(
            JSON_FIELD(s_framebuffer_json, renderpass),
            JSON_FIELD(s_framebuffer_json, attachments)
    );

public:
    bool create(std::vector<VkRenderPass>& renderpasses, resources::framebuffer_handle* handle) const;
};

struct s_swapchain_json {
public:
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    size_t imageCount = 1;

    JSON_FIELDS(
            JSON_FIELD_CONV(s_swapchain_json, format, utils::string_to_format),
            JSON_FIELD(s_swapchain_json, imageCount)
    );

public:
    s_swapchain_desc create() const;
};


}
}
}

#endif //GLADIUS_FRAMEBUFFER_DESC_H_H
