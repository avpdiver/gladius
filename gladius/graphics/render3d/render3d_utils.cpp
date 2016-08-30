//
// Created by alexa on 022 22 02 2016.
//

#include <cstring>
#include "render3d_globals.h"
#include "render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

bool check_extension(const char *extension_name, const std::vector<VkExtensionProperties> &available_extensions) {
    for (size_t i = 0; i < available_extensions.size(); ++i) {
        if (strcmp(available_extensions[i].extensionName, extension_name) == 0) {
            return true;
        }
    }
    return false;
}

bool get_supported_depth_format(VkPhysicalDevice gpu, VkFormat *format) {
    // Since all depth formats may be optional, we need to find a suitable depth format to use
    // Start with the highest precision packed format
    std::vector<VkFormat> depth_formats = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };

    for (auto &f : depth_formats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(gpu, f, &formatProps);
        // Format must support depth stencil attachment for optimal tiling
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            *format = f;
            return true;
        }
    }

    return false;
}

void set_image_layout(
    VkCommandBuffer cmd_buffer,
    VkImage image,
    VkImageAspectFlags aspect_mask,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkImageSubresourceRange &subresource_range) {

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
    if (old_layout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    // Old layout is color attachment
    // Make sure any writes to the color buffer have been finished
    if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    // Old layout is depth/stencil attachment
    // Make sure any writes to the depth/stencil buffer have been finished
    if (old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    // Old layout is transfer source
    // Make sure any reads from the image have been finished
    if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    // Old layout is shader read (sampler, input attachment)
    // Make sure any shader reads from the image have been finished
    if (old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    // Target layouts (new)

    // New layout is transfer destination (copy, blit)
    // Make sure any copyies to the image have been finished
    if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    // New layout is transfer source (copy, blit)
    // Make sure any reads from and writes to the image have been finished
    if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        image_memory_barrier.srcAccessMask =
            image_memory_barrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    // New layout is color attachment
    // Make sure any writes to the color buffer hav been finished
    if (new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    // New layout is depth attachment
    // Make sure any writes to depth/stencil buffer have been finished
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
            image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    // New layout is shader read (sampler, input attachment)
    // Make sure any writes to the image have been finished
    if (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    // Put barrier on top
    VkPipelineStageFlags src_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmd_buffer,
        src_stage_flags,
        dest_stage_flags,
        0,
        0, nullptr,
        0, nullptr,
        1, &image_memory_barrier);
}

void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                      VkImageLayout old_layout, VkImageLayout new_layout) {
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = aspect_mask;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = 1;
    subresource_range.layerCount = 1;
    set_image_layout(cmd_buffer, image, aspect_mask, old_layout, new_layout, subresource_range);
}

void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                      VkImageLayout old_layout, VkImageLayout new_layout, uint32_t base_level,
                      uint32_t level_count) {
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = aspect_mask;
    subresource_range.baseMipLevel = base_level;
    subresource_range.levelCount = level_count;
    subresource_range.layerCount = 1;
    set_image_layout(cmd_buffer, image, aspect_mask, old_layout, new_layout, subresource_range);
}

uint32_t get_memory_type(uint32_t typeBits, VkFlags requirements_mask) {
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            if ((vk_globals::gpu_memory_properties.memoryTypes[i].propertyFlags & requirements_mask) ==
                requirements_mask) {
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