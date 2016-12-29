//
// Created by alexa on 020 20 03 2016.
//

#include "../renderer3d.h"
#include "vulkan_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

bool create_command_pool(VkDevice device, uint32_t queue_index, VkCommandPool* pool) {
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.queueFamilyIndex = queue_index;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_VERIFY(vkCreateCommandPool(device, &command_pool_create_info, nullptr, pool));
    return true;
}

bool begin_command_buffer(VkCommandBuffer command_buffer) {
    VkCommandBufferBeginInfo cmd_info = {};
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_VERIFY(vkBeginCommandBuffer(command_buffer, &cmd_info));
    return true;
}

bool flush_command_buffer(VkQueue queue, VkCommandBuffer command_buffer) {
    VK_VERIFY(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VK_VERIFY(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
    VK_VERIFY(vkQueueWaitIdle(queue));

    return true;
}

bool create_command_buffer(VkDevice device, bool primary, VkCommandPool pool, uint32_t buffers_count, VkCommandBuffer* buffers) {
    VkCommandBufferAllocateInfo alloc_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr,
            pool,
            primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
            buffers_count
    };
    VK_VERIFY(vkAllocateCommandBuffers(device, &alloc_info, buffers));
    return true;
}

void destroy_command_buffer(VkDevice device, VkCommandBuffer handle, VkCommandPool pool) {
    vkFreeCommandBuffers(device, pool, 1, &handle);
}

}
}
}
}