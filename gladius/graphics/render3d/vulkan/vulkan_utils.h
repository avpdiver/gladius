//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_UTILS_H
#define GLADIUS_RENDER3D_UTILS_H

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

bool check_extension(const char *extension_name,
					 const std::vector<VkExtensionProperties> &available_extensions);

int find_memory_type(const VkPhysicalDeviceMemoryProperties& properties, uint32_t memory_type_bits, VkMemoryPropertyFlagBits property_flags);

uint32_t get_queue_family_index(VkQueueFlagBits queue_flags, const std::vector<VkQueueFamilyProperties>& queue_families_properties);

bool get_supported_depth_format(VkPhysicalDevice gpu, VkFormat *depth_format);

}
}
}
}

#endif //GLADIUS_RENDER3D_UTILS_H
