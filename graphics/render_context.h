//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_RENDER_CONTEXT_H
#define GLADIUS_RENDER_CONTEXT_H

#include "../core/math/vec4.h"

namespace gladius
{
    namespace geometry
    {
        class c_render_context
        {
        public:
            const math::vec4i& getViewport() const
            {
                return m_viewport;
            }

        private:
            math::vec4i m_viewport;
        };
    }
}

#endif //GLADIUS_RENDER_CONTEXT_H
