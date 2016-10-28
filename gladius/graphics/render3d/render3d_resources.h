//
// Created by alexa on 017 17 09 2016.
//

#ifndef GLADIUS_RENDER3D_RESOURCES_H
#define GLADIUS_RENDER3D_RESOURCES_H

#include <stdint.h>
#include <vulkan/vulkan.h>
#include "../../core/types.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_buffer_desc {
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    size_t size = 0;

    s_buffer_desc() = default;
    ~s_buffer_desc();
    MOVEABLE(s_buffer_desc);
};

struct s_texture_desc {
    uint32_t width;
	uint32_t height;
	uint32_t depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    VkFormat format;
	VkImageLayout image_layout;
    VkSampleCountFlagBits samples;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    s_texture_desc() = default;
    ~s_texture_desc();
    MOVEABLE(s_texture_desc);
};

constexpr size_t RENDER_CONTEXT_NUMBER = 3;

struct s_render_context {
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkSemaphore image_available_semaphore = VK_NULL_HANDLE;
    VkSemaphore rendering_finished_semaphore = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    handle_t staging_buffer;
};

struct s_thread_context {
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    s_render_context render_contexts[RENDER_CONTEXT_NUMBER];
};

bool get_current_render_context(s_render_context const** context);

}
}
}
}

#endif //GLADIUS_RENDER3D_RESOURCES_H
