//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_SWAPCHAIN_H
#define GLADIUS_RENDER3D_SWAPCHAIN_H

#include "../../core/window/window.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace swapchain
            {
                bool init(core::c_window *window);

                void shutdown();

                VkResult acquire_next_image(VkSemaphore semaphore);

                VkResult present(VkQueue queue, VkSemaphore semaphore);
            }
        }
    }
}

#endif //GLADIUS_RENDER3D_SWAPCHAIN_H
