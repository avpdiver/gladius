//
// Created by alexa on 025 25 12 2016.
//

#include <cassert>
#include "gpu_memory_allocator.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

c_gpu_memory_allocator::c_gpu_memory_allocator(const VkDevice &device, VkDeviceSize size) :
        m_chunk_allocator(device, size) {
}

c_block c_gpu_memory_allocator::alloc(VkDeviceSize size, VkDeviceSize alignment, uint32_t memory_type_index) {
    c_block block;
    // We search a "good" chunk
    for (auto &chunk : m_chunks) {
        if (chunk->memory_type_index() == memory_type_index) {
            if (chunk->alloc(size, alignment, block)) {
                return block;
            }
        }
    }

    m_chunks.emplace_back(m_chunk_allocator.alloc(size, memory_type_index));
    assert(m_chunks.back()->alloc(size, alignment, block));

    return block;
}

void c_gpu_memory_allocator::free(c_block &block) {
    for (auto &chunk : m_chunks) {
        if (chunk->is_in(block)) {
            chunk->free(block);
            return;
        }
    }
    assert(!"unable to deallocate the block");
}

}
}
}
}