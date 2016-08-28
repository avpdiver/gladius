//
// Created by alexa on 028 28 08 2016.
//

#ifndef GLADIUS_RENDER3D_SWAPCHAIN_H
#define GLADIUS_RENDER3D_SWAPCHAIN_H

#include <glm/vec2.hpp>

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            bool create_swap_chain(bool create_veiws = false);
            void shutdown_swap_chain();
        }
    }
}

#endif //GLADIUS_RENDER3D_SWAPCHAIN_H
