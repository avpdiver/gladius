//
// Created by alexa on 020 20 03 2016.
//

#include <iostream>
#include "render3d_globals.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace debug {

std::vector<const char *> validation_layer_names {
    "VK_LAYER_LUNARG_threading",
    "VK_LAYER_LUNARG_mem_tracker",
    "VK_LAYER_LUNARG_object_tracker",
    "VK_LAYER_LUNARG_draw_state",
    "VK_LAYER_LUNARG_param_checker",
    "VK_LAYER_LUNARG_swapchain",
    "VK_LAYER_LUNARG_device_limits",
    "VK_LAYER_LUNARG_image",
    "VK_LAYER_GOOGLE_unique_objects",
};

VkDebugReportCallbackEXT debug_report_callback = nullptr;

DEFINE_VK_PROC(CreateDebugReportCallbackEXT);
DEFINE_VK_PROC(DestroyDebugReportCallbackEXT);
DEFINE_VK_PROC(DebugReportMessageEXT);

VkBool32 callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type, uint64_t src_object,
                  size_t location, int32_t msg_code, const char *prefix, const char *msg,
                  void *user_data) {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        std::cerr << "ERROR: " << "[" << prefix << "] Code " << msg_code << " : " << msg << "\n";
    } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        std::cerr << "WARNING: " << "[" << prefix << "] Code " << msg_code << " : " << msg << "\n";
    } else {
        return false;
    }

    fflush(stdout);
    return false;
}

bool init(VkInstance instance) {
    GET_INSTANCE_PROC_ADDR(instance, CreateDebugReportCallbackEXT);
    GET_INSTANCE_PROC_ADDR(instance, DestroyDebugReportCallbackEXT);
    GET_INSTANCE_PROC_ADDR(instance, DebugReportMessageEXT);

    VkDebugReportCallbackCreateInfoEXT info;
    info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    info.pNext = nullptr;
    info.pfnCallback = (PFN_vkDebugReportCallbackEXT) callback;
    info.pUserData = nullptr;
    info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

    VK_VERIFY(CreateDebugReportCallbackEXT(instance, &info, nullptr, &debug_report_callback));

    return true;
}

void shutdown() {
    if (debug_report_callback != nullptr) {
        DestroyDebugReportCallbackEXT(vk_globals::instance, debug_report_callback, nullptr);
    }
}

const char *error_text(int result) {
    switch (result) {
        // todo : update to SDK 0.10.1
#define STR(r) case VK_ ##r: return #r
    STR(NOT_READY);
    STR(TIMEOUT);
    STR(EVENT_SET);
    STR(EVENT_RESET);
    STR(INCOMPLETE);
    STR(ERROR_OUT_OF_HOST_MEMORY);
    STR(ERROR_OUT_OF_DEVICE_MEMORY);
    STR(ERROR_INITIALIZATION_FAILED);
    STR(ERROR_DEVICE_LOST);
    STR(ERROR_MEMORY_MAP_FAILED);
    STR(ERROR_LAYER_NOT_PRESENT);
    STR(ERROR_EXTENSION_NOT_PRESENT);
    STR(ERROR_INCOMPATIBLE_DRIVER);
#undef STR
    default:
        return "UNKNOWN_ERROR";
    }
}

}
}
}
}
