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
        SET_ERROR(LOG_TYPE, "Faulid to get vk"#entrypoint, "");                     \
        return false;                                                               \
    }                                                                               \
}

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                       \
{                                                                                   \
    entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);    \
    if (entrypoint == nullptr)                                                      \
	{																                \
        SET_ERROR(LOG_TYPE, "Faulid to get vk"#entrypoint);                         \
        return false;                                                               \
    }                                                                               \
}

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/vec2.hpp>

#include "../../core/window/window.h"
#include "memory/gpu_memory_allocator.h"
#include "pipeline/pipeline_factory.h"

namespace gladius {
namespace graphics {
namespace render3d {

constexpr char const *LOG_TYPE = "RENDER3D";

class c_renderer3d {
public:
    struct s_device_queue {
        uint32_t index = UINT32_MAX;
        VkQueue handle = nullptr;
    };

    struct s_surface {
        VkSurfaceKHR surface;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct s_swapchain {
        uint32_t image_count;
        VkFormat format;
        VkSwapchainKHR handle;
        uint32_t width;
        uint32_t height;
        std::vector<VkImage> images;
        std::vector<VkImageView> views;
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<VkFence> fences;
    };

    struct s_semaphores {
        VkSemaphore present_complete;
        VkSemaphore render_complete;
        VkSubmitInfo submit_info;
    };

public:
    VkInstance m_instance;
    VkPhysicalDevice m_gpu;
    VkDevice m_device;

public:
    memory::c_gpu_memory_allocator *m_gpu_memory_allocator;

public:
    s_surface m_surface;
    s_swapchain m_swapchain;
    s_semaphores m_semaphores;

public:
    s_device_queue m_graphics_queue;

public:
    VkCommandPool m_command_pool;

public:
    VkPhysicalDeviceProperties m_device_properties;
    VkPhysicalDeviceFeatures m_device_features;
    VkPhysicalDeviceMemoryProperties m_gpu_memory_properties;
    std::vector<VkExtensionProperties> m_extensions;
    std::vector<VkQueueFamilyProperties> m_queue_families_properties;

private:
    s_pipeline m_forward_pipeline;

public:
    bool init(core::c_window *window);
    void shutdown();
    bool render();

private:
    bool build_command_buffer();
};

extern c_renderer3d renderer3d;

}
}
}

#include "vulkan/vulkan_debug.h"

#endif //GLADIUS_RENDERER3D_COMMON_H
