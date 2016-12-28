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

DEFINE_HANDLE(buffer_handle);

bool create_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memory_property, buffer_handle* handle);
void destroy_buffer(const buffer_handle handle);

}
}
}
}

#endif //GLADIUS_BUFFER_RESOURCE_H
