//
// Created by alexa on 022 22 02 2016.
//

#include <map>
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

int find_memory_type(uint32_t memory_type_bits, VkPhysicalDeviceMemoryProperties const &properties,
					 bool should_be_device_local) {

	auto lambda_get_memory_type = [&](VkMemoryPropertyFlags propertyFlags) -> int {
		for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
			if ((memory_type_bits & (1 << i)) &&
				((properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags))
				return i;
		return -1;
	};

	if (!should_be_device_local) {
		VkMemoryPropertyFlags optimal = VK_MEMORY_PROPERTY_HOST_CACHED_BIT |
										VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
										VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		VkMemoryPropertyFlags required = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
										 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		int type = lambda_get_memory_type(optimal);
		if (type == -1) {
			int result = lambda_get_memory_type(required);
			if (result == -1)
				assert(!"Memory type does not find");
			return result;
		}
		return type;
	} else {
		return lambda_get_memory_type(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
}

}
}
}
}