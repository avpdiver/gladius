//
// Created by alexa on 027 27 12 2016.
//

#include "pipeline_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

VkBool32 bool_to_uint32(const bool &v) {
    return v == true ? 1 : 0;
}

}
}
}