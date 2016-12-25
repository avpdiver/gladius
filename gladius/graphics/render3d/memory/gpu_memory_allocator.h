//
// Created by alexa on 025 25 12 2016.
//

#ifndef GLADIUS_GPU_MEMORY_ALLOCATOR_H
#define GLADIUS_GPU_MEMORY_ALLOCATOR_H

#include <vulkan/vulkan.h>

#include "block.h"
#include "chunk_allocator.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

class c_gpu_memory_allocator {
public:
    c_gpu_memory_allocator(VkDevice const &device, VkDeviceSize size);

public:
    c_block alloc(VkDeviceSize size, VkDeviceSize alignment, uint32_t memory_type_index);

    void free(c_block &block);

private:
    c_chunk_allocator m_chunk_allocator;
    std::vector<std::shared_ptr<c_chunk>> m_chunks;
};

}
}
}
}

#endif //GLADIUS_GPU_MEMORY_ALLOCATOR_H
