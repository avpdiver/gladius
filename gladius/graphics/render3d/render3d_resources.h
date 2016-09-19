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
    VkBuffer handle;
    VkDeviceMemory memory;
    size_t size;
};

struct s_texture_desc {
    uint32_t width, height, depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    VkFormat format;
    VkSampleCountFlagBits samples;
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
    VkImageLayout image_layout;
};

constexpr size_t RENDER_CONTEXT_NUMBER = 3;

struct s_render_context {
    VkCommandBuffer command_buffer = nullptr;
    VkSemaphore image_available_semaphore = nullptr;
    VkSemaphore rendering_finished_semaphore = nullptr;
    VkFence fence = nullptr;
    handle_t staging_buffer;
};

struct s_thread_context {
    VkCommandPool command_pool = nullptr;
    VkQueue queue = nullptr;
    s_render_context render_contexts[RENDER_CONTEXT_NUMBER];
};

bool get_current_render_context(s_render_context const** context);

}
}
}
}

#endif //GLADIUS_RENDER3D_RESOURCES_H
