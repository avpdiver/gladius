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

#define VK_CHECK(r) if (r != VK_SUCCESS) { SET_ERROR("%s", debug::error_text(r)); return false; }

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
                const char* error_text(int result);
            }

            extern VkInstance vk_instance;
            extern VkPhysicalDevice vk_gpu;
            extern VkDevice vk_device;
            extern VkQueue vk_queue;
            extern VkPhysicalDeviceMemoryProperties vk_device_memory_properties;
        }
    }
}

#endif //GLADIUS_RENDERER3D_COMMON_H
