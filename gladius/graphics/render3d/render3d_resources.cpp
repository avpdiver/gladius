//
// Created by alexa on 017 17 09 2016.
//

#include "render3d_globals.h"
#include "render3d_resources.h"
#include "render3d_command_buffer.h"
#include "render3d_buffer.h"
#include "render3d_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

thread_local size_t g_current_render_context_index = 0;
thread_local s_thread_context g_thread_context;

bool init() {
    VERIFY(create_command_pool(vk_globals::graphics_queue.index, &g_thread_context.command_pool));

    VkSemaphoreCreateInfo semaphore_create_info = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
            nullptr,                                      // const void*              pNext
            0                                             // VkSemaphoreCreateFlags   flags
    };

    VkFenceCreateInfo fence_create_info = {
            VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
            nullptr,                                          // const void                    *pNext
            VK_FENCE_CREATE_SIGNALED_BIT                      // VkFenceCreateFlags             flags
    };

    for (size_t i = 0; i < RENDER_CONTEXT_NUMBER; i++) {
        VERIFY(create_command_buffer(true, g_thread_context.command_pool, &(g_thread_context.render_contexts[i].command_buffer)));
        VK_VERIFY(vkCreateSemaphore(vk_globals::device, &semaphore_create_info, nullptr, &(g_thread_context.render_contexts[i].image_available_semaphore)));
        VK_VERIFY(vkCreateSemaphore(vk_globals::device, &semaphore_create_info, nullptr, &(g_thread_context.render_contexts[i].rendering_finished_semaphore)));
        VK_VERIFY(vkCreateFence(vk_globals::device, &fence_create_info, nullptr, &(g_thread_context.render_contexts[i].fence)));
        VERIFY(create_buffer(4000000, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                             &g_thread_context.render_contexts[i].staging_buffer));
    }

    return true;
}

bool get_current_render_context(s_render_context const** context) {
    if (g_thread_context.command_pool == nullptr) {
        VERIFY(init());
    }

    g_current_render_context_index = (++g_current_render_context_index) % RENDER_CONTEXT_NUMBER;
    const auto &current = g_thread_context.render_contexts[g_current_render_context_index];

    VK_VERIFY(vkWaitForFences(vk_globals::device, 1, &current.fence, VK_FALSE, 1000000000));
    vkResetFences(vk_globals::device, 1, &current.fence);

    (*context) = &current;
    return true;
}

}
}
}
}