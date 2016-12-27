//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_RENDER3D_SURFACE_H
#define GLADIUS_RENDER3D_SURFACE_H

#include "../../core/window/window.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool create_surface(core::c_window *window);
bool get_surface_capabilites();

}
}
}

#endif //GLADIUS_RENDER3D_SURFACE_H
