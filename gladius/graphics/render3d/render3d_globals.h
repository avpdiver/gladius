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

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/vec2.hpp>

#include "render3d_types.h"


namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace vk_globals
            {
                extern bool is_init;
                extern VkInstance instance;
                extern VkSurfaceKHR surface;
                extern VkPhysicalDevice gpu;
                extern VkDevice device;
                extern VkQueue queue;
                extern uint32_t graphics_queue_index;
                extern VkSurfaceFormatKHR surface_format;
                extern VkPhysicalDeviceMemoryProperties gpu_memory_properties;
                extern VkCommandBuffer setup_command_buffer;

                namespace swapchain_info
                {
                    struct s_swapchain_image_info
                    {
                        VkImage image;
                        VkImageView view;
                    };

                    extern VkFormat format;
                    extern VkSwapchainKHR swapchain;
                    extern glm::ivec2 image_size;
                    extern std::vector<s_swapchain_image_info> image_buffer;
                }

                namespace depth_buffer_info
                {
                    extern VkImage image;
                    extern VkImageView image_view;
                    extern VkFormat format;
                }

                struct s_thread_context
                {
                    VkQueue queue = nullptr;
                    VkCommandPool command_pool = nullptr;
                };
                extern thread_local s_thread_context thread_context;

                struct s_sync
                {
                    VkSemaphore present_semaphore = nullptr;
                    VkSemaphore render_semaphores = nullptr;
                };
                extern s_sync semaphores;
            }
        }
    }
}

#include "render3d_debug.h"

#endif //GLADIUS_RENDERER3D_COMMON_H
