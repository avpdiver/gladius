//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_FRAMEBUFFER_RESOURCE_H
#define GLADIUS_FRAMEBUFFER_RESOURCE_H

#include <vulkan/vulkan.h>
#include "../../../core/types.h"
#include "../render3d_texture.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_framebuffer_desc {
    struct s_attachment {
        texture_handle texture = INVALID_HANDLE;
        VkImageView image_view = VK_NULL_HANDLE;
        bool is_texture_owner = false;
    };
    s_attachment attachments[8];
    size_t attachmentCount = 0;
    VkFramebuffer handle = VK_NULL_HANDLE;

    s_framebuffer_desc() = default;
    ~s_framebuffer_desc();
    MOVEABLE(s_framebuffer_desc);
};

}
}
}
}

#endif //GLADIUS_FRAMEBUFFER_RESOURCE_H
