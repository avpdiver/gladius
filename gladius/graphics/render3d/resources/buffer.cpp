//
// Created by alexa on 018 18 09 2016.
//

#include <array>
#include <cstring>

#include "../../../core/memory/alloc_policies/lockfree_alloc.h"
#include "../../../core/memory/allocator.h"
#include "../renderer3d.h"
#include "../vulkan/vulkan_macros.h"

#include "buffer.h"
#include "resource_macros.h"


namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_buffer_desc {
public:
    VkBuffer handle = VK_NULL_HANDLE;
    memory::c_block memory_block;
    size_t size = 0;

public:
    s_buffer_desc() = default;
    ~s_buffer_desc();

public:
    NOT_MOVEABLE(s_buffer_desc);
    NOT_COPYABLE(s_buffer_desc);
};

static constexpr size_t RESOURCES_NUMBER = 32;

RESOURCE_POOL(s_buffer_desc, RESOURCES_NUMBER);

bool create_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memory_property, buffer_handle* handle) {
    VkBufferCreateInfo buffer_create_info = {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType        sType
            nullptr,                                          // const void            *pNext
            0,                                                // VkBufferCreateFlags    flags
            size,                                             // VkDeviceSize           size
            usage,                                            // VkBufferUsageFlags     usage
            VK_SHARING_MODE_EXCLUSIVE,                        // VkSharingMode          sharingMode
            0,                                                // uint32_t               queueFamilyIndexCount
            nullptr                                           // const uint32_t        *pQueueFamilyIndices
    };

    s_buffer_desc buf;
    buf.size = size;
    VkBuffer buffer;
    VK_VERIFY (vkCreateBuffer(renderer3d.m_device, &buffer_create_info, nullptr, &buffer));

    auto memory_block = renderer3d.m_gpu_memory_allocator->alloc(buf.handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_VERIFY (vkBindBufferMemory(renderer3d.m_device, buf.handle, buf.memory_block.m_memory, buf.memory_block.m_offset));

    s_buffer_desc* desc = (s_buffer_desc*)g_resource_pool.alloc(1);
    desc->size = size;
    desc->handle = buffer;
    desc->memory_block = memory_block;

    (*handle) = reinterpret_cast<buffer_handle>(desc);

    return true;
}

void destroy(s_buffer_desc* desc) {
    renderer3d.m_gpu_memory_allocator->free(desc->memory_block);
    vkDestroyBuffer(renderer3d.m_device, desc->handle, nullptr);
}

void destroy_buffer(const buffer_handle handle) {
    if (handle == INVALID_HANDLE) {
        return;
    }
    s_buffer_desc* desc = reinterpret_cast<s_buffer_desc*>(handle);
    destroy(desc);
    g_resource_pool.free(desc);
}

s_buffer_desc::~s_buffer_desc() {
    destroy(this);
}

}
}
}
}
