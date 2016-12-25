//
// Created by alexa on 018 18 09 2016.
//

#include <array>
#include <cstring>

#include "../../core/memory/allocator.h"
#include "../../core/memory/alloc_policies/lockfree_alloc.h"

#include "render3d_buffer.h"
#include "render3d_globals.h"
#include "render3d_resources.h"
#include "render3d_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

constexpr size_t RESOURCES_NUMBER = 32;

typedef typename std::aligned_storage<sizeof(s_buffer_desc), alignof(s_buffer_desc)>::type s_buffer_t;
static core::memory::c_allocator<
        std::array<s_buffer_t, RESOURCES_NUMBER>,
        core::memory::c_lockfree_alloc<sizeof(s_buffer_t), alignof(s_buffer_t)>> g_resource_pool;

bool alloc_memory(VkBuffer buffer, VkMemoryPropertyFlagBits property, VkDeviceMemory *memory) {
    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements(vk_globals::device, buffer, &buffer_memory_requirements);

    for (uint32_t i = 0; i < vk_globals::gpu_memory_properties.memoryTypeCount; ++i) {
        if ((buffer_memory_requirements.memoryTypeBits & (1 << i))
            && (vk_globals::gpu_memory_properties.memoryTypes[i].propertyFlags & property)) {

            VkMemoryAllocateInfo memory_allocate_info = {
                    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
                    nullptr,                                    // const void                            *pNext
                    buffer_memory_requirements.size,            // VkDeviceSize                           allocationSize
                    i                                           // uint32_t                               memoryTypeIndex
            };
            VK_VERIFY (vkAllocateMemory(vk_globals::device, &memory_allocate_info, nullptr, memory));
            return true;
        }
    }

    VERIFY_LOG(false, LOG_TYPE, "Failed allocate memory for buffer", "");
}

bool create_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memory_property, handle_t* handle) {
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

    VK_VERIFY (vkCreateBuffer(vk_globals::device, &buffer_create_info, nullptr, &buf.handle));
    VERIFY(alloc_memory(buf.handle, memory_property, &buf.memory));
    VK_VERIFY (vkBindBufferMemory(vk_globals::device, buf.handle, buf.memory, 0));

    s_buffer_desc* buffer = (s_buffer_desc*)g_resource_pool.alloc(1);
    (*buffer) = std::move(buf);
    (*handle) = reinterpret_cast<handle_t>(buffer);

    return true;
}

void destroy(s_buffer_desc* desc) {
	vkFreeMemory(vk_globals::device, desc->memory, nullptr);
	vkDestroyBuffer(vk_globals::device, desc->handle, nullptr);
}

void destroy_buffer(const handle_t& handle) {
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

DEFAULT_MOVE_IMPL(s_buffer_desc)

}
}
}
}
