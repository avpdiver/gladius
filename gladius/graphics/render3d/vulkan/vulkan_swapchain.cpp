//
// Created by alexa on 028 28 08 2016.
//

#include "../renderer3d.h"
#include "vulkan_swapchain.h"
#include "vulkan_utils.h"
#include "vulkan_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {

uint32_t get_swapchain_num_images(size_t images, VkSurfaceCapabilitiesKHR &surface_capabilities) {
    if ((surface_capabilities.maxImageCount > 0) && (images > surface_capabilities.maxImageCount)) {
        images = surface_capabilities.maxImageCount;
    }
    return images;
}

VkSurfaceFormatKHR get_swapchain_format(VkFormat format, std::vector<VkSurfaceFormatKHR> &surface_formats) {
    // If the list contains only one entry with undefined format
    // it means that there are no preferred surface formats and any can be chosen
    if ((surface_formats.size() == 1) &&
        (surface_formats[0].format == VK_FORMAT_UNDEFINED)) {
        return {format, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    }

    // Check if list contains most widely used R8 G8 B8 A8 format
    // with nonlinear color space
    for (VkSurfaceFormatKHR &surface_format : surface_formats) {
        if (surface_format.format == format) {
            return surface_format;
        }
    }

    // Return the first format from the list
    return {VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
}

VkExtent2D get_swapchain_extent(VkSurfaceCapabilitiesKHR &surface_capabilities) {
    // Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within defined confines
    if (surface_capabilities.currentExtent.width == static_cast<uint32_t>(-1)) {
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

VkImageUsageFlags get_swapchain_usage_flags(VkSurfaceCapabilitiesKHR &surface_capabilities) {
    // Color attachment flag must always be supported
    // We can define other usage flags but we always need to check if they are supported
    if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    SET_ERROR (LOG_TYPE, "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain!", "");
    return static_cast<VkImageUsageFlags>(-1);
}

VkSurfaceTransformFlagBitsKHR get_swapchain_transform(VkSurfaceCapabilitiesKHR &surface_capabilities) {
    // Sometimes images must be transformed before they are presented (i.e. due to device's orienation
    // being other than default orientation)
    // If the specified transform is other than current transform, presentation engine will transform image
    // during presentation operation; this operation may hit performance on some platforms
    // Here we don't want any transformations to occur so if the identity transform is supported use it
    // otherwise just use the same transform as current transform
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        return surface_capabilities.currentTransform;
    }
}

VkPresentModeKHR get_swapchain_present_mode(std::vector<VkPresentModeKHR> &present_modes) {
    // FIFO present mode is always available
    // MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
    for (VkPresentModeKHR &present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    for (VkPresentModeKHR &present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            return present_mode;
        }
    }
    SET_ERROR (LOG_TYPE, "FIFO present mode is not supported by the swap chain!", "");
    return static_cast<VkPresentModeKHR>(-1);
}

bool create_swap_chain(c_renderer3d* renderer) {
    if (renderer->m_device != nullptr) {
        vkDeviceWaitIdle(renderer->m_device);
    }

    shutdown_swap_chain(renderer);

    uint32_t desired_number_of_images = get_swapchain_num_images(renderer->m_swapchain.image_count, renderer->m_surface.capabilities);
    VERIFY_LOG(desired_number_of_images == renderer->m_swapchain.image_count,
               LOG_TYPE, "Requested %i swapchain images, but get %i", renderer->m_swapchain.image_count, desired_number_of_images);

    VkSurfaceFormatKHR desired_format = get_swapchain_format(renderer->m_swapchain.format, renderer->m_surface.formats);
    VERIFY_LOG(desired_number_of_images == renderer->m_swapchain.image_count,
               LOG_TYPE, "Requested %i swapchain format not supported", renderer->m_swapchain.format);

    VkExtent2D desired_extent = get_swapchain_extent(renderer->m_surface.capabilities);
    VkImageUsageFlags desired_usage = get_swapchain_usage_flags(renderer->m_surface.capabilities);
    VkSurfaceTransformFlagBitsKHR desired_transform = get_swapchain_transform(renderer->m_surface.capabilities);
    VkPresentModeKHR desired_present_mode = get_swapchain_present_mode(renderer->m_surface.present_modes);
    VkSwapchainKHR old_swap_chain = renderer->m_swapchain.handle;

    VERIFY (static_cast<int>(desired_usage) != -1);
    VERIFY (static_cast<int>(desired_present_mode) != -1);

    VkSwapchainCreateInfoKHR swap_chain_create_info = {
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
            nullptr,                                      // const void                    *pNext
            0,                                            // VkSwapchainCreateFlagsKHR      flags
            renderer->m_surface.surface,                  // VkSurfaceKHR                   surface
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

    VK_VERIFY (vkCreateSwapchainKHR(renderer->m_device, &swap_chain_create_info, nullptr, &(renderer->m_swapchain.handle)));

    if (old_swap_chain != nullptr) {
        vkDestroySwapchainKHR(renderer->m_device, old_swap_chain, nullptr);
    }

    renderer->m_swapchain.format = desired_format.format;
    renderer->m_swapchain.width = desired_extent.width;
    renderer->m_swapchain.height = desired_extent.height;

    uint32_t image_count = 0;
    VK_VERIFY(vkGetSwapchainImagesKHR(renderer->m_device, renderer->m_swapchain.handle, &image_count, nullptr));
    VERIFY_LOG(image_count > 0, LOG_TYPE, "Could not get swap chain images!", "");

    renderer->m_swapchain.images.resize(image_count);
    VK_VERIFY (vkGetSwapchainImagesKHR(renderer->m_device, renderer->m_swapchain.handle, &image_count,
                                       &(renderer->m_swapchain.images[0])));


    renderer->m_swapchain.views.resize(renderer->m_swapchain.images.size());
    for (size_t i = 0; i < renderer->m_swapchain.images.size(); ++i) {
        VkImageViewCreateInfo image_view_create_info =
                {
                        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
                        nullptr,                                    // const void                    *pNext
                        0,                                          // VkImageViewCreateFlags         flags
                        renderer->m_swapchain.images[i],            // VkImage                        image
                        VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
                        renderer->m_swapchain.format,               // VkFormat                       format
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

        VK_VERIFY (vkCreateImageView(renderer->m_device, &image_view_create_info, nullptr,
                                     &renderer->m_swapchain.views[i]));
    }

    return true;
}

void shutdown_swap_chain(c_renderer3d* renderer) {
    if (renderer->m_swapchain.handle != nullptr) {
        vkDestroySwapchainKHR(renderer->m_device, renderer->m_swapchain.handle, nullptr);
        renderer->m_swapchain.handle = nullptr;
    }
}

bool swap_chain_acquire_next_image(c_renderer3d* renderer, VkSemaphore semaphore, uint32_t *image_index) {
    VkResult result = vkAcquireNextImageKHR(renderer->m_device, renderer->m_swapchain.handle, UINT64_MAX, semaphore, (VkFence)nullptr, image_index);
    switch (result) {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            create_swap_chain(renderer);
            return true;
        default:
            SET_ERROR (LOG_TYPE, "Problem occurred during swap chain image acquisition!", "");
            return false;
    }
    return true;
}


}
}
}