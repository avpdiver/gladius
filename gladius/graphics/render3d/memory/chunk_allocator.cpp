//
// Created by alexa on 025 25 12 2016.
//

#include <vulkan/vulkan.h>
#include <cassert>
#include <math.h>

#include "chunk_allocator.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {


VkDeviceSize next_power_of_two(VkDeviceSize size) {
    VkDeviceSize power = (VkDeviceSize)std::log2l(size) + 1;
    return (VkDeviceSize)1 << power;
}

bool isPowerOfTwo(VkDeviceSize size) {
    VkDeviceSize mask = 0;
    VkDeviceSize power = (VkDeviceSize)std::log2l(size);

    for(VkDeviceSize i = 0; i < power; ++i)
        mask += (VkDeviceSize)1 << i;

    return !(size & mask);
}

c_chunk_allocator::c_chunk_allocator(const VkDevice &device, VkDeviceSize size) :
        m_device(device),
        m_size(size) {
    assert(isPowerOfTwo(size));
}

std::unique_ptr<c_chunk> c_chunk_allocator::alloc(VkDeviceSize size, int memory_type_index) {
    size = (size > m_size) ? next_power_of_two(size) : m_size;
    return std::make_unique<c_chunk>(m_device, size, memory_type_index);
}

}
}
}
}