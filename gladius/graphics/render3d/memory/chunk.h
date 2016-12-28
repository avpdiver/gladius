//
// Created by alexa on 025 25 12 2016.
//

#ifndef GLADIUS_RENDERER3D_MEMORY_CHUNK_H
#define GLADIUS_RENDERER3D_MEMORY_CHUNK_H

#include <vulkan/vulkan.h>
#include <vector>

#include "block.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

class c_chunk {
public:
    MOVEABLE(c_chunk);
    NOT_COPYABLE(c_chunk);

public:
    c_chunk(VkDevice const &device, VkDeviceSize size, uint32_t memory_type_index);
    ~c_chunk();

public:
    bool alloc(VkDeviceSize size, VkDeviceSize alignment, c_block &block);
    void free(c_block const &block);

public:
    bool is_in(c_block const &block) const;
    uint32_t memory_type_index() const;

protected:
    VkDevice m_device;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size;
    uint32_t m_memory_type_index;
    std::vector <c_block> m_blocks;
    void *m_ptr = nullptr;
};

}
}
}
}

#endif //GLADIUS_RENDERER3D_MEMORY_CHUNK_H
