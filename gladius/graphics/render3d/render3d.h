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

struct render3d_desc {
    const char* pipeline_file = "pipeline.json";
    bool enable_validation = false;
};

bool init(core::c_window *window, const render3d_desc& desc);
void shutdown();
bool render();

}
}
}

#endif //GLADIUS_RENDERER3D_H
