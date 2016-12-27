//
// Created by alexa on 027 27 12 2016.
//

#include "render3d_globals.h"

#include "render3d_surface.h"
#include "render3d_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {

DEFINE_VK_PROC(GetPhysicalDeviceSurfaceSupportKHR);
DEFINE_VK_PROC(GetPhysicalDeviceSurfaceFormatsKHR);

namespace vk_globals {
s_surface surface;
}

bool create_surface(core::c_window *window) {

    GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceFormatsKHR);

    const core::s_window_info *window_sys_info = window->get_window_info();

#ifdef PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_create_info = {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                  sType
            nullptr,                                          // const void                      *pNext
            0,                                                // VkWin32SurfaceCreateFlagsKHR     flags
            window_sys_info->instance,                        // HINSTANCE                        hinstance
            window_sys_info->handle                           // HWND                             hwnd
    };
    VK_VERIFY (vkCreateWin32SurfaceKHR(vk_globals::instance, &surface_create_info, nullptr, &(vk_globals::surface.surface)));

#elif PLATFORM_LINUX
    VkXcbSurfaceCreateInfoKHR surface_create_info = {
        VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                  sType
        nullptr,                                          // const void                      *pNext
        0,                                                // VkXcbSurfaceCreateFlagsKHR       flags
        window_sys_info->connection,                      // xcb_connection_t*                connection
        window_sys_info->handle                           // xcb_window_t                     window
    };
    VK_VERIFY (vkCreateXcbSurfaceKHR(vk_globals::instance, &surface_create_info, nullptr, &(vk_globals::surface)));
#endif

    return true;
}

bool get_surface_capabilites() {
    VK_VERIFY (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_globals::gpu, vk_globals::surface.surface,
                                                         &vk_globals::surface.capabilities));

    // Acquiring Supported Surface Formats
    uint32_t formats_count;
    VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_globals::gpu, vk_globals::surface.surface, &formats_count,
                                                   nullptr));
    VERIFY_LOG(formats_count > 0, LOG_TYPE, "Error occurred during presentation surface formats enumeration!", "");

    vk_globals::surface.formats.resize(formats_count);
    VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_globals::gpu, vk_globals::surface.surface, &formats_count,
                                                   &vk_globals::surface.formats[0]));

    // Acquiring Supported Present Modes
    uint32_t present_modes_count;
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_globals::gpu, vk_globals::surface.surface,
                                                        &present_modes_count, nullptr));
    VERIFY_LOG(present_modes_count > 0, LOG_TYPE,
               "Error occurred during presentation surface present modes enumeration!", "");

    vk_globals::surface.present_modes.resize(present_modes_count);
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_globals::gpu, vk_globals::surface.surface,
                                                        &present_modes_count, &vk_globals::surface.present_modes[0]));

    return true;
}

}
}
}