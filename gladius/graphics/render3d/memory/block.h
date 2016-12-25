#ifndef GLADIUS_RENDERER3D_MEMORY_BLOCK_H
#define GLADIUS_RENDERER3D_MEMORY_BLOCK_H

#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

    class c_block {
    public:
        VkDeviceMemory m_memory;
        VkDeviceSize m_offset;
        VkDeviceSize m_size;
        bool m_free = false;
        void *m_ptr = nullptr;

    public:
        bool operator ==(c_block const &block);
    };

}
}
}
}

#endif