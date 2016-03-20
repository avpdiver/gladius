//
// Created by alexa on 023 23 02 2016.
//

#ifndef GLADIUS_RENDERER3D_TEXTURE_H
#define GLADIUS_RENDERER3D_TEXTURE_H

#include <stdint.h>

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace resources
            {
                bool load_texture(const char *filename, uint32_t format);
            }
        }
    }
}

#endif //GLADIUS_RENDERER3D_TEXTURE_H
