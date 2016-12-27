//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_BUFFER_RESOURCE_H
#define GLADIUS_BUFFER_RESOURCE_H

#include <vulkan/vulkan.h>
#include "../../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_buffer_desc {
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    size_t size = 0;

    s_buffer_desc() = default;

    ~s_buffer_desc();

    MOVEABLE(s_buffer_desc);
};

}
}
}
}

#endif //GLADIUS_BUFFER_RESOURCE_H
