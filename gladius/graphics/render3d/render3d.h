//
// Created by pav on 18.02.16.
//

#ifndef GLADIUS_RENDERER3D_H
#define GLADIUS_RENDERER3D_H

#include "../../core/logger/logger.h"
#include "../../core/window/window.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool init(core::c_window *window);
void shutdown();
bool render();

}
}
}

#endif //GLADIUS_RENDERER3D_H
