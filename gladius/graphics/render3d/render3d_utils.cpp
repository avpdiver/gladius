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

}
}
}
}