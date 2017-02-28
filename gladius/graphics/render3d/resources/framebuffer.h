//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_FRAMEBUFFER_RESOURCE_H
#define GLADIUS_FRAMEBUFFER_RESOURCE_H

#include <vulkan/vulkan.h>
#include "../../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

DEFINE_HANDLE(framebuffer_handle);

struct s_framebuffer_attachment {
    texture_handle texture = INVALID_HANDLE;
    VkImageView image_view = VK_NULL_HANDLE;
    bool is_texture_owner = false;
};

bool create_framebuffer(uint32_t width, uint32_t height,
                        const std::vector<s_framebuffer_attachment>& attachments, VkRenderPass render_pass,
                        framebuffer_handle *handle);
void destroy_framebuffer(const framebuffer_handle handle);

}
}
}
}

#endif //GLADIUS_FRAMEBUFFER_RESOURCE_H
