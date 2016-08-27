//
// Created by alexa on 022 22 02 2016.
//

#include <cstring>
#include "render3d_globals.h"
#include "render3d_utils.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace utils
            {
                bool check_extension (const char *extension_name, const std::vector<VkExtensionProperties>& available_extensions)
                {
                    for (size_t i = 0; i < available_extensions.size (); ++i)
                    {
                        if (strcmp (available_extensions[i].extensionName, extension_name) == 0)
                        {
                            return true;
                        }
                    }
                    return false;
                }

                bool check_physical_device_properties (VkPhysicalDevice physical_device,
                                                       uint32_t& selected_graphics_queue_family_index,
                                                       uint32_t& selected_present_queue_family_index,
                                                       const std::vector<const char *>& device_extensions)
                {
                    uint32_t extensions_count = 0;
                    if ((vkEnumerateDeviceExtensionProperties (physical_device, nullptr, &extensions_count, nullptr)
                         != VK_SUCCESS)
                        || (extensions_count == 0))
                    {
                        SET_ERRORF ("Error occurred during physical device %d extensions enumeration!", physical_device);
                        return false;
                    }

                    std::vector<VkExtensionProperties> available_extensions (extensions_count);
                    if (vkEnumerateDeviceExtensionProperties (physical_device, nullptr, &extensions_count, &available_extensions[0])
                        != VK_SUCCESS)
                    {
                        SET_ERRORF ("Error occurred during physical device %d extensions enumeration!", physical_device);
                        return false;
                    }

                    for (size_t i = 0; i < device_extensions.size (); ++i)
                    {
                        if (!check_extension (device_extensions[i], available_extensions))
                        {
                            SET_ERRORF ("Physical device %d doesn't support extension named \"%s\"!", physical_device, device_extensions[i]);
                            return false;
                        }
                    }

                    VkPhysicalDeviceProperties device_properties;
                    VkPhysicalDeviceFeatures device_features;

                    vkGetPhysicalDeviceProperties (physical_device, &device_properties);
                    vkGetPhysicalDeviceFeatures (physical_device, &device_features);

                    uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);

                    if ((major_version < 1) && (device_properties.limits.maxImageDimension2D < 4096))
                    {
                        SET_ERRORF ("Physical device %d doesn't support required parameters!", physical_device);
                        return false;
                    }

                    uint32_t queue_families_count = 0;
                    vkGetPhysicalDeviceQueueFamilyProperties (physical_device, &queue_families_count, nullptr);
                    if (queue_families_count == 0)
                    {
                        SET_ERRORF ("Physical device %d doesn't have any handle families!", physical_device);
                        return false;
                    }

                    std::vector<VkQueueFamilyProperties> queue_family_properties (queue_families_count);
                    std::vector<VkBool32> queue_present_support (queue_families_count);

                    vkGetPhysicalDeviceQueueFamilyProperties (physical_device, &queue_families_count, &queue_family_properties[0]);

                    uint32_t graphics_queue_family_index = UINT32_MAX;
                    uint32_t present_queue_family_index = UINT32_MAX;

                    for (uint32_t i = 0; i < queue_families_count; ++i)
                    {
                        vkGetPhysicalDeviceSurfaceSupportKHR (physical_device, i, vk_globals::surface, &queue_present_support[i]);

                        if ((queue_family_properties[i].queueCount > 0)
                            && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                        {
                            // Select first handle that supports graphics
                            if (graphics_queue_family_index == UINT32_MAX)
                            {
                                graphics_queue_family_index = i;
                            }

                            // If there is handle that supports both graphics and present - prefer it
                            if (queue_present_support[i])
                            {
                                selected_graphics_queue_family_index = i;
                                selected_present_queue_family_index = i;
                                return true;
                            }
                        }
                    }

                    // We don't have handle that supports both graphics and present so we have to use separate queues
                    for (uint32_t i = 0; i < queue_families_count; ++i)
                    {
                        if (queue_present_support[i])
                        {
                            present_queue_family_index = i;
                            break;
                        }
                    }

                    // If this device doesn't support queues with graphics and present capabilities don't use it
                    if ((graphics_queue_family_index == UINT32_MAX) || (present_queue_family_index == UINT32_MAX))
                    {
                        SET_ERRORF ("Could not find handle families with required properties on physical device %d!", physical_device);
                        return false;
                    }

                    selected_graphics_queue_family_index = graphics_queue_family_index;
                    selected_present_queue_family_index = present_queue_family_index;
                    return true;
                }

                uint32_t get_swapchain_num_images (VkSurfaceCapabilitiesKHR& surface_capabilities)
                {
                    // Set of images defined in a swap chain may not always be available for application to render to:
                    // One may be displayed and one may wait in a handle to be presented
                    // If application wants to use more images at the same time it must ask for more images
                    uint32_t image_count = surface_capabilities.minImageCount + 1;
                    if ((surface_capabilities.maxImageCount > 0) && (image_count > surface_capabilities.maxImageCount))
                    {
                        image_count = surface_capabilities.maxImageCount;
                    }
                    return image_count;
                }

                VkSurfaceFormatKHR get_swapchain_format (std::vector<VkSurfaceFormatKHR>& surface_formats)
                {
                    // If the list contains only one entry with undefined format
                    // it means that there are no preferred surface formats and any can be chosen
                    if ((surface_formats.size () == 1) &&
                        (surface_formats[0].format == VK_FORMAT_UNDEFINED))
                    {
                        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
                    }

                    // Check if list contains most widely used R8 G8 B8 A8 format
                    // with nonlinear color space
                    for (VkSurfaceFormatKHR& surface_format : surface_formats)
                    {
                        if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM)
                        {
                            return surface_format;
                        }
                    }

                    // Return the first format from the list
                    return surface_formats[0];
                }

                VkExtent2D get_swapchain_extent (VkSurfaceCapabilitiesKHR& surface_capabilities)
                {
                    // Special value of surface extent is width == height == -1
                    // If this is so we define the size by ourselves but it must fit within defined confines
                    if (surface_capabilities.currentExtent.width == static_cast<uint32_t>(-1))
                    {
                        VkExtent2D swap_chain_extent = {640, 480};

                        swap_chain_extent.width = std::max(swap_chain_extent.width, surface_capabilities.minImageExtent.width);
                        swap_chain_extent.height = std::max(swap_chain_extent.height, surface_capabilities.minImageExtent.height);

                        swap_chain_extent.width = std::min(swap_chain_extent.width, surface_capabilities.maxImageExtent.width);
                        swap_chain_extent.height = std::min(swap_chain_extent.height, surface_capabilities.maxImageExtent.height);

                        return swap_chain_extent;
                    }

                    // Most of the cases we define size of the swap_chain images equal to current window's size
                    return surface_capabilities.currentExtent;
                }

                VkImageUsageFlags get_swapchain_usage_flags(VkSurfaceCapabilitiesKHR &surface_capabilities)
                {
                    // Color attachment flag must always be supported
                    // We can define other usage flags but we always need to check if they are supported
                    if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                    {
                        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                    }
                    SET_ERROR ("VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain!");
                    return static_cast<VkImageUsageFlags>(-1);
                }

                VkSurfaceTransformFlagBitsKHR get_swapchain_transform(VkSurfaceCapabilitiesKHR &surface_capabilities)
                {
                    // Sometimes images must be transformed before they are presented (i.e. due to device's orienation
                    // being other than default orientation)
                    // If the specified transform is other than current transform, presentation engine will transform image
                    // during presentation operation; this operation may hit performance on some platforms
                    // Here we don't want any transformations to occur so if the identity transform is supported use it
                    // otherwise just use the same transform as current transform
                    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                    {
                        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                    }
                    else
                    {
                        return surface_capabilities.currentTransform;
                    }
                }

                VkPresentModeKHR get_swapchain_present_mode(std::vector<VkPresentModeKHR> &present_modes)
                {
                    // FIFO present mode is always available
                    // MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
                    for (VkPresentModeKHR& present_mode : present_modes)
                    {
                        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                        {
                            return present_mode;
                        }
                    }
                    for (VkPresentModeKHR& present_mode : present_modes)
                    {
                        if (present_mode == VK_PRESENT_MODE_FIFO_KHR)
                        {
                            return present_mode;
                        }
                    }
                    SET_ERROR ("FIFO present mode is not supported by the swap chain!");
                    return static_cast<VkPresentModeKHR>(-1);
                }

                bool get_supported_depth_format (VkPhysicalDevice gpu, VkFormat *format)
                {
                    // Since all depth formats may be optional, we need to find a suitable depth format to use
                    // Start with the highest precision packed format
                    std::vector<VkFormat> depth_formats = {
                        VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_FORMAT_D32_SFLOAT,
                        VK_FORMAT_D24_UNORM_S8_UINT,
                        VK_FORMAT_D16_UNORM_S8_UINT,
                        VK_FORMAT_D16_UNORM
                    };

                    for (auto& f : depth_formats)
                    {
                        VkFormatProperties formatProps;
                        vkGetPhysicalDeviceFormatProperties (gpu, f, &formatProps);
                        // Format must support depth stencil attachment for optimal tiling
                        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        {
                            *format = f;
                            return true;
                        }
                    }

                    return false;
                }

                void set_image_layout (
                    VkCommandBuffer cmd_buffer,
                    VkImage image,
                    VkImageAspectFlags aspect_mask,
                    VkImageLayout old_layout,
                    VkImageLayout new_layout,
                    VkImageSubresourceRange& subresource_range)
                {
                    // Create an image barrier object
                    VkImageMemoryBarrier image_memory_barrier = {};
                    image_memory_barrier.oldLayout = old_layout;
                    image_memory_barrier.newLayout = new_layout;
                    image_memory_barrier.image = image;
                    image_memory_barrier.subresourceRange = subresource_range;

                    // Source layouts (old)

                    // Undefined layout
                    // Only allowed as initial layout!
                    // Make sure any writes to the image have been finished
                    if (old_layout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                    }

                    // Old layout is color attachment
                    // Make sure any writes to the color buffer have been finished
                    if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    }

                    // Old layout is depth/stencil attachment
                    // Make sure any writes to the depth/stencil buffer have been finished
                    if (old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    }

                    // Old layout is transfer source
                    // Make sure any reads from the image have been finished
                    if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    }

                    // Old layout is shader read (sampler, input attachment)
                    // Make sure any shader reads from the image have been finished
                    if (old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    }

                    // Target layouts (new)

                    // New layout is transfer destination (copy, blit)
                    // Make sure any copyies to the image have been finished
                    if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                    {
                        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    }

                    // New layout is transfer source (copy, blit)
                    // Make sure any reads from and writes to the image have been finished
                    if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask =
                            image_memory_barrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
                        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    }

                    // New layout is color attachment
                    // Make sure any writes to the color buffer hav been finished
                    if (new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    }

                    // New layout is depth attachment
                    // Make sure any writes to depth/stencil buffer have been finished
                    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.dstAccessMask =
                            image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    }

                    // New layout is shader read (sampler, input attachment)
                    // Make sure any writes to the image have been finished
                    if (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    }

                    // Put barrier on top
                    VkPipelineStageFlags src_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    VkPipelineStageFlags dest_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                    // Put barrier inside setup command buffer
                    vkCmdPipelineBarrier (
                        cmd_buffer,
                        src_stage_flags,
                        dest_stage_flags,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &image_memory_barrier);
                }

                void set_image_layout (VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                       VkImageLayout old_layout, VkImageLayout new_layout)
                {
                    VkImageSubresourceRange subresource_range = {};
                    subresource_range.aspectMask = aspect_mask;
                    subresource_range.baseMipLevel = 0;
                    subresource_range.levelCount = 1;
                    subresource_range.layerCount = 1;
                    set_image_layout (cmd_buffer, image, aspect_mask, old_layout, new_layout, subresource_range);
                }

                void set_image_layout (VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                                       VkImageLayout old_layout, VkImageLayout new_layout, uint32_t base_level,
                                       uint32_t level_count)
                {
                    VkImageSubresourceRange subresource_range = {};
                    subresource_range.aspectMask = aspect_mask;
                    subresource_range.baseMipLevel = base_level;
                    subresource_range.levelCount = level_count;
                    subresource_range.layerCount = 1;
                    set_image_layout (cmd_buffer, image, aspect_mask, old_layout, new_layout, subresource_range);
                }

                uint32_t get_memory_type (uint32_t typeBits, VkFlags requirements_mask)
                {
                    for (uint32_t i = 0; i < 32; i++)
                    {
                        if ((typeBits & 1) == 1)
                        {
                            if ((vk_globals::gpu_memory_properties.memoryTypes[i].propertyFlags & requirements_mask) ==
                                requirements_mask)
                            {
                                return i;
                            }
                        }
                        typeBits >>= 1;
                    }
                    return -1;
                }
            }
        }
    }
}