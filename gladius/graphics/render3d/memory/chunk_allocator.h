//
// Created by alexa on 025 25 12 2016.
//

#ifndef GLADIUS_RENDERER3D_MEMORY_CHUNK_ALLOCATOR_H
#define GLADIUS_RENDERER3D_MEMORY_CHUNK_ALLOCATOR_H

#include <memory>

#include "../../../core/types.h"
#include "chunk.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

class c_chunk_allocator
{
public:
    NOT_COPYABLE(c_chunk_allocator);
    NOT_MOVEABLE(c_chunk_allocator);

public:
    c_chunk_allocator(VkDevice const &device, VkDeviceSize size);

    // if size > mSize, allocate to the next power of 2
    std::unique_ptr<c_chunk> alloc(VkDeviceSize size, int memory_type_index);

private:
    VkDevice m_device;
    VkDeviceSize m_size;
};

}
}
}
}

#endif //GLADIUS_RENDERER3D_MEMORY_CHUNK_ALLOCATOR_H
