//
// Created by pav on 18.02.16.
//

#ifndef GLADIUS_RENDERER3D_H
#define GLADIUS_RENDERER3D_H

#include "../core/logging/logging.h"
#include "../core/window/window.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            bool init(core::c_window* window, bool enable_validation);
            void shutdown();

            namespace resources
            {
                size_t create_command_buffer(bool primary = true);
                void  destroy_command_buffer(size_t handle);
            }
        };
    }
}

#endif //GLADIUS_RENDERER3D_H
