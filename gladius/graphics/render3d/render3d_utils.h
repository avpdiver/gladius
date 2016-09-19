//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_UTILS_H
#define GLADIUS_RENDER3D_UTILS_H

#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

bool check_extension(const char *extension_name,
                     const std::vector<VkExtensionProperties> &available_extensions);

}
}
}
}

#endif //GLADIUS_RENDER3D_UTILS_H
