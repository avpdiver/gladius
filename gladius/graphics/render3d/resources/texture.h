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

DEFINE_HANDLE(texture_handle);

bool create_image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples,
                  VkImageUsageFlags usage, VkImage *image);
bool create_image_view(VkImage image, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkImageView *view);


bool create_texture(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
                    texture_handle *handle);
bool load_texture(const char *filename, texture_handle *handle);
void destroy_texture(texture_handle handle);

VkImageView get_texture_image_view(texture_handle handle);

}
}
}
}

#endif //GLADIUS_TEXTURE_RESOURCE_H
