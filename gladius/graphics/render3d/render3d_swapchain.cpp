//
// Created by alexa on 028 28 08 2016.
//

#include "render3d_globals.h"
#include "render3d_swapchain.h"
#include "render3d_utils.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace vk_globals
            {
                s_swapchain swapchain = {};
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
                    VkExtent2D swap_chain_extent = {1, 1};

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

            bool create_swap_chain(bool create_veiws)
            {
                if (vk_globals::device != nullptr)
                {
                    vkDeviceWaitIdle (vk_globals::device);
                }

                shutdown_swap_chain();

                // Acquiring Surface Capabilities
                VkSurfaceCapabilitiesKHR surface_capabilities;
                VK_VERIFY (vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vk_globals::gpu, vk_globals::surface,
                                                                      &surface_capabilities));

                // Acquiring Supported Surface Formats
                uint32_t formats_count;
                VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR (vk_globals::gpu, vk_globals::surface, &formats_count,
                                                                nullptr));
                VERIFY_LOG(formats_count > 0, "Error occurred during presentation surface formats enumeration!");

                std::vector<VkSurfaceFormatKHR> surface_formats (formats_count);
                VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR (vk_globals::gpu, vk_globals::surface, &formats_count,
                                                                &surface_formats[0]));

                // Acquiring Supported Present Modes
                uint32_t present_modes_count;
                VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR (vk_globals::gpu, vk_globals::surface,
                                                                     &present_modes_count, nullptr));
                VERIFY_LOG(present_modes_count > 0, "Error occurred during presentation surface present modes enumeration!");

                std::vector<VkPresentModeKHR> present_modes (present_modes_count);
                VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR (vk_globals::gpu, vk_globals::surface,
                                                                     &present_modes_count, &present_modes[0]));


                uint32_t desired_number_of_images = get_swapchain_num_images (surface_capabilities);
                VkSurfaceFormatKHR desired_format = get_swapchain_format (surface_formats);
                VkExtent2D desired_extent = get_swapchain_extent (surface_capabilities);
                VkImageUsageFlags desired_usage = get_swapchain_usage_flags (surface_capabilities);
                VkSurfaceTransformFlagBitsKHR desired_transform = get_swapchain_transform (surface_capabilities);
                VkPresentModeKHR desired_present_mode = get_swapchain_present_mode (present_modes);
                VkSwapchainKHR old_swap_chain = vk_globals::swapchain.handle;

                VERIFY (static_cast<int>(desired_usage) != -1);
                VERIFY (static_cast<int>(desired_present_mode) != -1);

                VkSwapchainCreateInfoKHR swap_chain_create_info = {
                        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
                        nullptr,                                      // const void                    *pNext
                        0,                                            // VkSwapchainCreateFlagsKHR      flags
                        vk_globals::surface,                          // VkSurfaceKHR                   surface
                        desired_number_of_images,                     // uint32_t                       minImageCount
                        desired_format.format,                        // VkFormat                       imageFormat
                        desired_format.colorSpace,                    // VkColorSpaceKHR                imageColorSpace
                        desired_extent,                               // VkExtent2D                     imageExtent
                        1,                                            // uint32_t                       imageArrayLayers
                        desired_usage,                                // VkImageUsageFlags              imageUsage
                        VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
                        0,                                            // uint32_t                       queueFamilyIndexCount
                        nullptr,                                      // const uint32_t                *pQueueFamilyIndices
                        desired_transform,                            // VkSurfaceTransformFlagBitsKHR  preTransform
                        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
                        desired_present_mode,                         // VkPresentModeKHR               presentMode
                        VK_TRUE,                                      // VkBool32                       clipped
                        old_swap_chain                                // VkSwapchainKHR                 oldSwapchain
                };

                VK_VERIFY (vkCreateSwapchainKHR (vk_globals::device, &swap_chain_create_info, nullptr,
                                                 &(vk_globals::swapchain.handle)));
                if (old_swap_chain != nullptr)
                {
                    vkDestroySwapchainKHR (vk_globals::device, old_swap_chain, nullptr);
                }

                vk_globals::swapchain.format = desired_format.format;

                uint32_t image_count = 0;
                VK_VERIFY(vkGetSwapchainImagesKHR (vk_globals::device, vk_globals::swapchain.handle, &image_count, nullptr));
                VERIFY_LOG(image_count > 0, "Could not get swap chain images!");

                vk_globals::swapchain.images.resize (image_count);
                VK_VERIFY (vkGetSwapchainImagesKHR (vk_globals::device, vk_globals::swapchain.handle, &image_count,
                                                    &(vk_globals::swapchain.images[0])));

                if (create_veiws)
                {
                    vk_globals::swapchain.views.resize(vk_globals::swapchain.images.size());
                    for ( size_t i = 0; i < vk_globals::swapchain.images.size(); ++i)
                    {
                        VkImageViewCreateInfo image_view_create_info =
                                {
                                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
                                nullptr,                                    // const void                    *pNext
                                0,                                          // VkImageViewCreateFlags         flags
                                vk_globals::swapchain.images[i],            // VkImage                        image
                                VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
                                vk_globals::swapchain.format,               // VkFormat                       format
                                {                                           // VkComponentMapping             components
                                        VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             r
                                        VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             g
                                        VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             b
                                        VK_COMPONENT_SWIZZLE_IDENTITY       // VkComponentSwizzle             a
                                },
                                {                                           // VkImageSubresourceRange        subresourceRange
                                        VK_IMAGE_ASPECT_COLOR_BIT,          // VkImageAspectFlags             aspectMask
                                        0,                                  // uint32_t                       baseMipLevel
                                        1,                                  // uint32_t                       levelCount
                                        0,                                  // uint32_t                       baseArrayLayer
                                        1                                   // uint32_t                       layerCount
                                }
                        };

                        VK_VERIFY (vkCreateImageView(vk_globals::device, &image_view_create_info, nullptr,
                                                     &vk_globals::swapchain.views[i]));
                    }
                }

                return true;
            }

            void shutdown_swap_chain()
            {
                if (vk_globals::swapchain.handle != nullptr)
                {
                    vkDestroySwapchainKHR (vk_globals::device, vk_globals::swapchain.handle, nullptr);
                    vk_globals::swapchain.handle = nullptr;
                }
            }
        }
    }
}