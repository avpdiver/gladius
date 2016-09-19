//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_UTILS_H
#define GLADIUS_RENDER3D_UTILS_H

#include <string>
#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

bool check_extension(const char *extension_name,
                     const std::vector<VkExtensionProperties> &available_extensions);

VkSampleCountFlagBits get_sample_count(size_t samples);

VkFormat string_to_format(const std::string &str);

}
}
}
}

#endif //GLADIUS_RENDER3D_UTILS_H
