//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_RENDER_CONTEXT_H
#define GLADIUS_RENDER_CONTEXT_H

#include "glm/vec4.hpp"

namespace gladius
{
    namespace geometry
    {
        class c_render_context
        {
        public:
            const glm::ivec4& getViewport() const
            {
                return m_viewport;
            }

        private:
            glm::ivec4 m_viewport;
        };
    }
}

#endif //GLADIUS_RENDER_CONTEXT_H
