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
                bool check_extension (const char *extension_name,
                                      const std::vector<VkExtensionProperties>& available_extensions);

                bool check_physical_device_properties (VkPhysicalDevice physical_device,
                                                       uint32_t& selected_graphics_queue_family_index,
                                                       uint32_t& selected_present_queue_family_index,
                                                       const std::vector<const char *>& device_extensions);

                uint32_t get_swapchain_num_images (VkSurfaceCapabilitiesKHR& surface_capabilities);

                VkSurfaceFormatKHR get_swapchain_format (std::vector<VkSurfaceFormatKHR>& surface_formats);

                VkExtent2D get_swapchain_extent (VkSurfaceCapabilitiesKHR& surface_capabilities);

                VkImageUsageFlags get_swapchain_usage_flags(VkSurfaceCapabilitiesKHR &surface_capabilities);

                VkSurfaceTransformFlagBitsKHR get_swapchain_transform(VkSurfaceCapabilitiesKHR &surface_capabilities);

                VkPresentModeKHR get_swapchain_present_mode(std::vector<VkPresentModeKHR> &present_modes);

                void set_image_layout (VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                       VkImageLayout old_layout, VkImageLayout new_layout,
                                       VkImageSubresourceRange& subresource_range);

                void set_image_layout (VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                       VkImageLayout old_layout, VkImageLayout new_layout);

                void set_image_layout (VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                       VkImageLayout old_layout, VkImageLayout new_layout, uint32_t base_level,
                                       uint32_t level_count);

                uint32_t get_memory_type (uint32_t typeBits, VkFlags requirements_mask);

                bool get_supported_depth_format (VkPhysicalDevice gpu, VkFormat *format);
            }
        }
    }
}

#endif //GLADIUS_RENDER3D_UTILS_H
