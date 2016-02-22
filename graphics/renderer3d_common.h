//
// Created by alexa on 021 21 02 2016.
//

#ifndef GLADIUS_RENDERER3D_COMMON_H
#define GLADIUS_RENDERER3D_COMMON_H


#ifdef PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef PLATFORM_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#ifdef PLATFORM_MACOS
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#ifdef PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <vector>
#include <vulkan/vulkan.h>

#include "../core/logging/logging.h"
#include "../core/window/window.h"

#define VK_VERIFY(r) { VkResult result = (r); if (result != VK_SUCCESS) { SET_ERROR("%s", debug::error_text(result)); return false; } }

#define DEFINE_VK_PROC(entrypoint)  PFN_vk##entrypoint entrypoint

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                    \
{                                                                                   \
    entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(inst, "vk"#entrypoint); \
    if (entrypoint == nullptr)                                                      \
	{																                \
        SET_ERROR("Faulid to get vk"#entrypoint);                                   \
        return false;                                                               \
    }                                                                               \
}

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                       \
{                                                                                   \
    entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);    \
    if (entrypoint == nullptr)                                                      \
	{																                \
        SET_ERROR("Faulid to get vk"#entrypoint);                                   \
        return false;                                                               \
    }                                                                               \
}

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            namespace debug
            {
                extern std::vector<const char *> validation_layer_names;

                bool init();

                void shutdown();

                const char *error_text(int result);
            }

            namespace swapchain
            {
                bool init(core::c_window *window);
                void shutdown();
            }

            namespace utils
            {
                void set_image_layout(VkCommandBuffer command_buffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                                      VkImageLayout new_image_layout);
            }

            namespace resources
            {
                bool init();
                void shutdown();

                VkCommandBuffer get_command_buffer(size_t handle);

                extern VkCommandBuffer vk_setup_command_buffer;
            }

            extern VkInstance vk_instance;
            extern VkSurfaceKHR vk_surface;
            extern VkPhysicalDevice vk_gpu;
            extern VkDevice vk_device;
            extern VkQueue vk_queue;
            extern VkPhysicalDeviceMemoryProperties vk_device_memory_properties;
            extern uint32_t vk_graphics_queue_index;
            extern uint32_t vk_present_queue_index;
            extern VkSurfaceFormatKHR vk_surface_format;

        }
    }
}

#endif //GLADIUS_RENDERER3D_COMMON_H
