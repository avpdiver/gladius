//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_UTILS_H
#define GLADIUS_RENDER3D_UTILS_H

#include <vulkan/vulkan.h>

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace utils
            {
                void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                      VkImageLayout old_layout, VkImageLayout new_layout,
                                      VkImageSubresourceRange &subresource_range);

                void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                      VkImageLayout old_layout, VkImageLayout new_layout);

                void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                      VkImageLayout old_layout, VkImageLayout new_layout, uint32_t base_level,
                                      uint32_t level_count);

                uint32_t get_memory_type(uint32_t typeBits, VkFlags requirements_mask);

                bool get_supported_depth_format(VkPhysicalDevice gpu, VkFormat *format);
            }
        }
    }
}

#endif //GLADIUS_RENDER3D_UTILS_H