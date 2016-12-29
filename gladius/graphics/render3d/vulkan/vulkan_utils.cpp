//
// Created by alexa on 022 22 02 2016.
//

#include <map>
#include <cstring>

#include "../renderer3d.h"
#include "vulkan_utils.h"

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

int find_memory_type(const VkPhysicalDeviceMemoryProperties& properties, uint32_t memory_type_bits, VkMemoryPropertyFlagBits property_flags) {
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i) {
        if ((memory_type_bits & (1 << i))
            && (properties.memoryTypes[i].propertyFlags & property_flags)) {
            return i;
        }
    }
    return -1;
}

uint32_t get_queue_family_index(VkQueueFlagBits queue_flags, const std::vector<VkQueueFamilyProperties>& queue_families_properties)
{
    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (queue_flags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families_properties.size()); i++)
        {
            if ((queue_families_properties[i].queueFlags & queue_flags) && ((queue_families_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                return i;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if (queue_flags & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families_properties.size()); i++)
        {
            if ((queue_families_properties[i].queueFlags & queue_flags) && ((queue_families_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queue_families_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                return i;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families_properties.size()); i++)
    {
        if (queue_families_properties[i].queueFlags & queue_flags)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

bool get_supported_depth_format(VkPhysicalDevice gpu, VkFormat *depthFormat) {
    // Since all depth formats may be optional, we need to find a suitable depth format to use
    // Start with the highest precision packed format
    std::vector<VkFormat> depth_formats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
    };

    for (auto &format : depth_formats) {
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_props);
        if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            *depthFormat = format;
            return true;
        }
    }

    return false;
}

}
}
}
}