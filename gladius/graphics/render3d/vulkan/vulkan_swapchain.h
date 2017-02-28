//
// Created by alexa on 028 28 08 2016.
//

#ifndef GLADIUS_RENDER3D_SWAPCHAIN_H
#define GLADIUS_RENDER3D_SWAPCHAIN_H

#include <glm/vec2.hpp>

namespace gladius {
namespace graphics {
namespace render3d {

struct s_swapchain_desc {
    VkFormat format;
    size_t imageCount;
};

bool create_swap_chain(c_renderer3d* renderer);
void shutdown_swap_chain(c_renderer3d* renderer);
bool swap_chain_acquire_next_image(c_renderer3d* renderer, VkSemaphore semaphore, uint32_t *image_index);

}
}
}

#endif //GLADIUS_RENDER3D_SWAPCHAIN_H
