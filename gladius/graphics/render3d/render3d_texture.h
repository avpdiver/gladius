//
// Created by alexa on 023 23 02 2016.
//

#ifndef GLADIUS_RENDERER3D_TEXTURE_H
#define GLADIUS_RENDERER3D_TEXTURE_H

#include <stdint.h>
#include <vulkan/vulkan.h>
#include "../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

DEFINE_HANDLE(texture_handle);

bool create_image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples,
                  VkImageUsageFlags usage, VkImage *image);

bool create_image_view(VkImage image, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkImageView *view);


bool create_texture(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
                    texture_handle *handle);

bool load_texture(const char *filename, texture_handle *handle);

void destroy_texture(const texture_handle handle);

}
}
}
}

#endif //GLADIUS_RENDERER3D_TEXTURE_H
