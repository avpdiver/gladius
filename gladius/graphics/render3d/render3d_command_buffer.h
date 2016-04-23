//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_COMMAND_BUFFER_H
#define GLADIUS_RENDER3D_COMMAND_BUFFER_H

#include <cstddef>

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace resources
            {
                bool create_command_pool(uint32_t queue_index);

                VkCommandBuffer create_command_buffer(bool primary = true);

                bool create_command_buffers(uint32_t count, VkCommandBuffer* buffers, bool primary = true);

                void destroy(VkCommandBuffer handle);

                void destroy(uint32_t count, VkCommandBuffer* buffers);

                bool begin_command_buffer(VkCommandBuffer command_buffer);

                bool flush_command_buffer(VkQueue queue, VkCommandBuffer command_buffer);
            }
        }
    }
}

#endif //GLADIUS_RENDER3D_COMMAND_BUFFER_H
