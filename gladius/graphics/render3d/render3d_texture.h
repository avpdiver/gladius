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

bool create_image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels,
                  uint32_t array_layers, VkImage *image);
bool create_image_view(VkImage image, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkImageView* view);

bool load_texture(const char *filename, handle_t& handle);

}
}
}
}

#endif //GLADIUS_RENDERER3D_TEXTURE_H
