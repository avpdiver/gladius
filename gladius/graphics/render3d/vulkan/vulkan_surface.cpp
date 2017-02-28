//
// Created by alexa on 027 27 12 2016.
//

#include "../renderer3d.h"

#include "vulkan_surface.h"
#include "vulkan_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {

DEFINE_VK_PROC(GetPhysicalDeviceSurfaceSupportKHR);
DEFINE_VK_PROC(GetPhysicalDeviceSurfaceFormatsKHR);

bool get_surface_capabilites(c_renderer3d* renderer) {
    VK_VERIFY (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->m_gpu, renderer->m_surface.surface,
                                                         &renderer->m_surface.capabilities));

    // Acquiring Supported Surface Formats
    uint32_t formats_count;
    VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->m_gpu, renderer->m_surface.surface, &formats_count,
                                                   nullptr));
    VERIFY_LOG(formats_count > 0, LOG_TYPE, "Error occurred during presentation surface formats enumeration!", "");

    renderer->m_surface.formats.resize(formats_count);
    VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->m_gpu, renderer->m_surface.surface, &formats_count,
                                                   &renderer->m_surface.formats[0]));

    // Acquiring Supported Present Modes
    uint32_t present_modes_count;
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->m_gpu, renderer->m_surface.surface,
                                                        &present_modes_count, nullptr));
    VERIFY_LOG(present_modes_count > 0, LOG_TYPE,
               "Error occurred during presentation surface present modes enumeration!", "");

    renderer->m_surface.present_modes.resize(present_modes_count);
    VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->m_gpu, renderer->m_surface.surface,
                                                        &present_modes_count, &renderer->m_surface.present_modes[0]));

    return true;
}

bool create_surface(c_renderer3d* renderer, core::c_window *window) {

    GET_INSTANCE_PROC_ADDR(renderer->m_instance, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(renderer->m_instance, GetPhysicalDeviceSurfaceFormatsKHR);

    const core::s_window_info *window_sys_info = window->get_window_info();

#ifdef PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_create_info = {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                  sType
            nullptr,                                          // const void                      *pNext
            0,                                                // VkWin32SurfaceCreateFlagsKHR     flags
            window_sys_info->instance,                        // HINSTANCE                        hinstance
            window_sys_info->handle                           // HWND                             hwnd
    };
    VK_VERIFY (vkCreateWin32SurfaceKHR(renderer->m_instance, &surface_create_info, nullptr, &(renderer->m_surface.surface)));

#elif PLATFORM_LINUX
    VkXcbSurfaceCreateInfoKHR surface_create_info = {
        VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                  sType
        nullptr,                                          // const void                      *pNext
        0,                                                // VkXcbSurfaceCreateFlagsKHR       flags
        window_sys_info->connection,                      // xcb_connection_t*                connection
        window_sys_info->handle                           // xcb_window_t                     window
    };
    VK_VERIFY (vkCreateXcbSurfaceKHR(renderer->m_instance, &surface_create_info, nullptr, &(renderer->m_surface.surface)));
#endif

    return get_surface_capabilites(renderer);
}


}
}
}