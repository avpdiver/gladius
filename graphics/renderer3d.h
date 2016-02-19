//
// Created by pav on 18.02.16.
//

#ifndef GLADIUS_RENDERER3D_H
#define GLADIUS_RENDERER3D_H

#include <vulkan/vulkan.h>
#include "../core/window/window.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            namespace debug
            {
                extern int validation_layer_count;
                extern const char *validation_layer_names[];

                bool init(VkInstance instance);
                void shutdown(VkInstance instance);
            }

            bool init(core::c_window* window, bool enable_validation);
            void shutdown();
        };
    }
}

#endif //GLADIUS_RENDERER3D_H
