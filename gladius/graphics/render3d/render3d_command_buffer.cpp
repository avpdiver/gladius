//
// Created by alexa on 020 20 03 2016.
//

#include "render3d_globals.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace resources
            {
                bool create_command_pool()
                {
                    VkCommandPoolCreateInfo command_pool_create_info = {};
                    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    command_pool_create_info.pNext = NULL;
                    command_pool_create_info.queueFamilyIndex = vk_globals::graphics_queue.index;
                    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                    VK_VERIFY(vkCreateCommandPool(vk_globals::device, &command_pool_create_info, nullptr,
                                                  &(vk_globals::thread_context.command_pool)));
                    return true;
                }

                bool begin_command_buffer(VkCommandBuffer command_buffer)
                {
                    VkCommandBufferBeginInfo cmd_info = {};
                    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    VK_VERIFY(vkBeginCommandBuffer(command_buffer, &cmd_info));
                    return true;
                }

                bool flush_command_buffer(VkQueue queue, VkCommandBuffer command_buffer)
                {
                    VK_VERIFY(vkEndCommandBuffer(command_buffer));

                    VkSubmitInfo submit_info = {};
                    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    submit_info.commandBufferCount = 1;
                    submit_info.pCommandBuffers = &command_buffer;

                    VK_VERIFY(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
                    VK_VERIFY(vkQueueWaitIdle(queue));

                    return true;
                }

                VkCommandBuffer create_command_buffer(bool primary)
                {
                    VkCommandBufferAllocateInfo alloc_info = {};
                    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    alloc_info.pNext = nullptr;
                    alloc_info.commandPool = vk_globals::thread_context.command_pool;
                    alloc_info.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
                    alloc_info.commandBufferCount = 1;

                    VkCommandBuffer command_buffer;
                    VK_VERIFY_RETURN(vkAllocateCommandBuffers(vk_globals::device, &alloc_info, &command_buffer), nullptr);
                    return command_buffer;
                }

                void destroy(VkCommandBuffer handle)
                {
                    vkFreeCommandBuffers(vk_globals::device, vk_globals::thread_context.command_pool, 1, &handle);
                }
            }
        }
    }
}