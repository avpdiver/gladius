//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_PIPELINE_UTILS_H
#define GLADIUS_PIPELINE_UTILS_H

#include <vulkan/vulkan.h>

#include "../../../core/meta/json.h"
#include "../render3d_globals.h"

namespace gladius {
namespace graphics {
namespace render3d {

template<typename OUT_TYPE, typename IN_TYPE>
OUT_TYPE get_surface_width(const IN_TYPE& v) {
    return static_cast<OUT_TYPE>(vk_globals::surface.capabilities.currentExtent.width * v);
}

template<typename OUT_TYPE, typename IN_TYPE>
OUT_TYPE get_surface_height(const IN_TYPE& v) {
    return static_cast<OUT_TYPE>(vk_globals::surface.capabilities.currentExtent.height * v);
}

static constexpr auto float_width_func = get_surface_width<float, float>;
static constexpr auto float_height_func = get_surface_height<float, float>;

static constexpr auto uint32_width_func = get_surface_width<uint32_t, float>;
static constexpr auto uint32_height_func = get_surface_height<uint32_t, float>;

VkBool32 bool_to_uint32(const bool& v);

}
}
}

#endif //GLADIUS_PIPELINE_UTILS_H
