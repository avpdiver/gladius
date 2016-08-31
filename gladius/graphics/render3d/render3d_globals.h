//
// Created by alexa on 021 21 02 2016.
//

#ifndef GLADIUS_RENDERER3D_COMMON_H
#define GLADIUS_RENDERER3D_COMMON_H

#ifdef PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef PLATFORM_LINUX
//#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_XCB_KHR
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
        SET_ERROR("Faulid to get vk"#entrypoint, "");                               \
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

namespace gladius {
namespace graphics {
namespace render3d {
namespace vk_globals {

extern bool is_init;

extern VkInstance instance;
extern VkSurfaceKHR surface;
extern VkPhysicalDevice gpu;
extern VkDevice device;

struct s_device_queue {
	uint32_t index = UINT32_MAX;
	VkQueue handle = nullptr;
};

extern s_device_queue graphics_queue;
extern s_device_queue present_queue;

extern VkSurfaceFormatKHR surface_format;
extern VkPhysicalDeviceMemoryProperties gpu_memory_properties;

struct s_swapchain {
	VkFormat format;
	VkSwapchainKHR handle;
	std::vector<VkImage> images;
	std::vector<VkImageView> views;
};
extern s_swapchain swapchain;

extern std::vector<VkCommandBuffer> present_command_buffers;

struct s_depth_buffer_info {
	VkImage image;
	VkImageView image_view;
	VkFormat format;
};
extern s_depth_buffer_info depth_buffer_info;

struct s_thread_context {
	VkQueue queue = nullptr;
	VkCommandPool command_pool = nullptr;
};
extern thread_local s_thread_context thread_context;

struct s_sync_semaphores {
	VkSemaphore image_available_semaphore = nullptr;
	VkSemaphore rendering_finished_semaphore = nullptr;
};
extern s_sync_semaphores semaphores;

}
}
}
}

#include "render3d_debug.h"

#endif //GLADIUS_RENDERER3D_COMMON_H
