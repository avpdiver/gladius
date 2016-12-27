//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_RENDER3D_BUFFER_H
#define GLADIUS_RENDER3D_BUFFER_H

#include <stdint.h>
#include <vulkan/vulkan.h>
#include "../../core/types.h"

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

#endif //GLADIUS_RENDER3D_BUFFER_H


