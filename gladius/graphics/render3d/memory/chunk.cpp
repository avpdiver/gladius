//
// Created by alexa on 025 25 12 2016.
//

#include <vulkan/vulkan.h>
#include <algorithm>

#include "chunk.h"
#include "../render3d_macros.h"
#include "../render3d_globals.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

c_chunk::c_chunk(const VkDevice &device, VkDeviceSize size, uint32_t memory_type_index) :
        m_device(device),
        m_size(size),
        m_memory_type_index(memory_type_index) {
    VkMemoryAllocateInfo memory_allocate_info = MEMORY_ALLOCATE_INFO(size, memory_type_index);

    VK_ASSERT (vkAllocateMemory(vk_globals::device, &memory_allocate_info, nullptr, &m_memory));

    c_block block = {
            m_memory,
            0,
            size,
            true
    };

    if((vk_globals::gpu_memory_properties.memoryTypes[memory_type_index].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        VK_ASSERT(vkMapMemory(device, m_memory, 0, VK_WHOLE_SIZE, 0, &m_ptr));
    }
    
    m_blocks.emplace_back(block);
}

uint32_t c_chunk::memory_type_index() const {
    return m_memory_type_index;
}

bool c_chunk::is_in(const c_block &block) const {
    return block.m_memory == m_memory;
}

void c_chunk::free(const c_block &block) {
    auto block_it(std::find(m_blocks.begin(), m_blocks.end(), block));
    assert(block_it != m_blocks.end());
    block_it->m_free = true;
}

bool c_chunk::alloc(VkDeviceSize size, VkDeviceSize alignment, c_block &block) {

    if (size > m_size) {
        return false;
    }

    for (uint32_t i = 0; i < m_blocks.size(); ++i) {
        if (!m_blocks[i].m_free) {
            continue;
        }

        // Compute virtual size after taking care about offsetAlignment
        uint32_t new_size = m_blocks[i].m_size;
        if (m_blocks[i].m_offset % alignment != 0) {
            new_size -= alignment - m_blocks[i].m_offset % alignment;
        }

        if (new_size < size) {
            continue;
        }

        // We compute offset and size that care about alignment (for this c_block)
        m_blocks[i].m_size = new_size;
        if (m_blocks[i].m_offset % alignment != 0) {
            m_blocks[i].m_offset += alignment - m_blocks[i].m_offset % alignment;
        }

        // Compute the ptr address
        if (m_ptr != nullptr) {
            m_blocks[i].m_ptr = (uint8_t *) m_ptr + m_blocks[i].m_offset;
        }

        // if perfect match
        if (m_blocks[i].m_size == size) {
            m_blocks[i].m_free = false;
            block = m_blocks[i];
            return true;
        }

        c_block nextBlock = {
                m_memory,
                m_blocks[i].m_offset + size,
                m_blocks[i].m_size - size,
                true,
        };

        m_blocks.emplace_back(nextBlock); // We add the newBlock

        m_blocks[i].m_size = size;
        m_blocks[i].m_free = false;

        block = m_blocks[i];

        return true;
    }

    return false;
}

c_chunk::~c_chunk() {
    vkFreeMemory(m_device, m_memory, nullptr);
}

}
}
}
}
