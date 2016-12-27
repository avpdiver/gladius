//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_TEXTURE_RESOURCE_H
#define GLADIUS_TEXTURE_RESOURCE_H

#include <vulkan/vulkan.h>
#include "../../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_texture_desc {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    VkFormat format;
    VkImageLayout image_layout;
    VkSampleCountFlagBits samples;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    s_texture_desc() = default;

    ~s_texture_desc();

    MOVEABLE(s_texture_desc);
};

}
}
}
}

#endif //GLADIUS_TEXTURE_RESOURCE_H
