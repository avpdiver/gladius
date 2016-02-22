//
// Created by alexa on 022 22 02 2016.
//

#include <thread>
#include <mutex>
#include "renderer3d.h"
#include "renderer3d_common.h"

#define VK_VERIFY_RETURN(r, ret) { VkResult result = (r); if (result != VK_SUCCESS) { SET_ERROR("%s", debug::error_text(result)); return ret; } }

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            namespace resources
            {
                VkCommandPool  vk_command_pool = nullptr;
                VkCommandBuffer vk_setup_command_buffer = nullptr;

                struct s_thread_context
                {
                    std::vector<VkCommandBuffer> vk_command_buffers;
                };

                std::unordered_map<std::thread::id, s_thread_context> thread_contexts;

                bool create_command_pool()
                {
                    VkCommandPoolCreateInfo command_pool_create_info = {};
                    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    command_pool_create_info.pNext = NULL;
                    command_pool_create_info.queueFamilyIndex = vk_graphics_queue_index;
                    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                    VK_VERIFY(vkCreateCommandPool(vk_device, &command_pool_create_info, nullptr, &vk_command_pool));
                    return true;
                }

                size_t create_command_buffer(bool primary)
                {
                    auto& thread_context = thread_contexts[std::this_thread::get_id()];

                    VkCommandBufferAllocateInfo alloc_info = {};
                    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    alloc_info.pNext = nullptr;
                    alloc_info.commandPool = vk_command_pool;
                    alloc_info.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
                    alloc_info.commandBufferCount = 1;

                    VkCommandBuffer  command_buffer;
                    VK_VERIFY_RETURN(vkAllocateCommandBuffers(vk_device, &alloc_info, &command_buffer), -1);
                    thread_context.vk_command_buffers.push_back(command_buffer);

                    return (thread_context.vk_command_buffers.size() - 1);
                }

                void destroy_command_buffer(size_t handle)
                {
                    auto& thread_context = thread_contexts[std::this_thread::get_id()];

                    if (handle >= thread_context.vk_command_buffers.size())
                        return;

                    vkFreeCommandBuffers(vk_device, vk_command_pool, 1, thread_context.vk_command_buffers.data() + handle);
                    thread_context.vk_command_buffers.erase(thread_context.vk_command_buffers.begin() + handle);
                }

                VkCommandBuffer get_command_buffer(size_t handle)
                {
                    const auto& thread_context = thread_contexts[std::this_thread::get_id()];
                    if (handle >= thread_context.vk_command_buffers.size())
                        return nullptr;
                    return thread_context.vk_command_buffers[handle];
                }

                bool init()
                {
                    if (!create_command_pool())
                    {
                        return false;
                    }

                    size_t id = create_command_buffer();
                    VERIFY(id != -1, "Failed create main thread command buffer");
                    vk_setup_command_buffer = get_command_buffer(id);

                    return true;
                }

                void shutdown()
                {
                    if (vk_command_pool != nullptr)
                    {
                        for (auto& e : thread_contexts)
                        {
                            if (!e.second.vk_command_buffers.empty())
                            {
                                vkFreeCommandBuffers(vk_device, vk_command_pool, e.second.vk_command_buffers.size(),
                                                     e.second.vk_command_buffers.data());
                            }
                        }
                        thread_contexts.clear();
                        vkDestroyCommandPool(vk_device, vk_command_pool, nullptr);
                    }
                }
            }
        };
    }
}
