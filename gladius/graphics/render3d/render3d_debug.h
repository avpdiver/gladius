//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_DEBUG_H
#define GLADIUS_RENDER3D_DEBUG_H

#include <vector>
#include "../../core/logger/logger.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace debug {

extern std::vector<const char *> validation_layer_names;

bool init();
void shutdown();
const char *error_text(int result);

}
}
}
}

#endif //GLADIUS_RENDER3D_DEBUG_H
