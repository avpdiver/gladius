//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_RENDER3D_FRAMEBUFFER_H
#define GLADIUS_RENDER3D_FRAMEBUFFER_H

#include <vector>
#include "resources/framebuffer_resource.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

DEFINE_HANDLE(framebuffer_handle);

bool create_framebuffer(uint32_t width, uint32_t height,
                        const std::vector<s_framebuffer_desc::s_attachment>& attachments, VkRenderPass render_pass,
                        framebuffer_handle *handle);

void destroy_framebuffer(const framebuffer_handle handle);

}
}
}
}

#endif //GLADIUS_RENDER3D_FRAMEBUFFER_H
