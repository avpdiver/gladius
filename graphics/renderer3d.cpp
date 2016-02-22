//
// Created by pav on 18.02.16.
//


#include <SDL2/SDL_syswm.h>
#include "renderer3d.h"
#include "renderer3d_common.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            bool enable_validation = false;
            VkInstance vk_instance = nullptr;
            VkSurfaceKHR  vk_surface = nullptr;
            VkPhysicalDevice vk_gpu = nullptr;
            VkDevice vk_device = nullptr;
            VkQueue vk_queue = nullptr;
            VkPhysicalDeviceMemoryProperties vk_device_memory_properties;
            uint32_t vk_graphics_queue_index = -1;
            uint32_t vk_present_queue_index = -1;
            VkSurfaceFormatKHR vk_surface_format = {};

            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceSupportKHR);
            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceFormatsKHR);

            bool create_instance(const char* app_name)
            {
                std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#ifdef PLATFORM_WINDOWS
                extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef PLATFORM_LINUX
                extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#ifdef PLATFORM_MACOS
                extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#ifdef PLATFORM_ANDROID
                extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

                if (extensions.size() == 0)
                {
                    SET_ERROR("Unknown surface %u");
                    return false;
                }

                VkApplicationInfo application_info = {};
                application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                application_info.pApplicationName = app_name;
                application_info.pEngineName = "gladius";
                application_info.apiVersion = VK_API_VERSION; //VK_MAKE_VERSION(1, 0, 2);


                VkInstanceCreateInfo instance_create_info = {};
                instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instance_create_info.pNext = nullptr;
                instance_create_info.pApplicationInfo = &application_info;

                if (extensions.size() > 0)
                {
                    if (enable_validation)
                    {
                        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                    }
                    instance_create_info.enabledExtensionCount = (uint32_t)extensions.size();
                    instance_create_info.ppEnabledExtensionNames = extensions.data();
                }

                if (enable_validation)
                {
                    instance_create_info.enabledLayerCount = debug::validation_layer_names.size();
                    instance_create_info.ppEnabledLayerNames = debug::validation_layer_names.data();
                }

                VK_VERIFY(vkCreateInstance(&instance_create_info, nullptr, &vk_instance));

                GET_INSTANCE_PROC_ADDR(vk_instance, GetPhysicalDeviceSurfaceSupportKHR);
                GET_INSTANCE_PROC_ADDR(vk_instance, GetPhysicalDeviceSurfaceFormatsKHR);

                return true;
            }

            bool create_surface(core::c_window* window)
            {
#ifdef PLATFORM_WINDOWS
                VkWin32SurfaceCreateInfoKHR surface_create_info = {};
                surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                surface_create_info.hinstance = GetModuleHandle(nullptr);
                surface_create_info.hwnd = window->get_system_info()->info.win.window;
                VK_VERIFY(vkCreateWin32SurfaceKHR(vk_instance, &surface_create_info, nullptr, &vk_surface));
#endif

#if defined (PLATFORM_LINUX) || defined(PLATFORM_MACOS)
                VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
		        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		        surfaceCreateInfo.connection = connection;
		        surfaceCreateInfo.window = window;
		        VK_VERIFY(vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#endif

#ifdef PLATFORM_ANDROID
                VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
		        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		        surfaceCreateInfo.window = window;
		        VK_VERIFY(vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface));
#endif
                return true;
            }

            bool create_device(uint32_t queue_index, bool enable_validation)
            {

                std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

                float queue_priorities[] = {0.0f};
                VkDeviceQueueCreateInfo queue_create_info = {};
                queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = queue_index;
                queue_create_info.queueCount = 1;
                queue_create_info.pQueuePriorities = queue_priorities;

                VkDeviceCreateInfo device_create_info = {};
                device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device_create_info.pNext = nullptr;
                device_create_info.queueCreateInfoCount = 1;
                device_create_info.pQueueCreateInfos = &queue_create_info;
                device_create_info.pEnabledFeatures = nullptr;

                if (extensions.size() > 0)
                {
                    device_create_info.enabledExtensionCount = (uint32_t) extensions.size();
                    device_create_info.ppEnabledExtensionNames = extensions.data();
                }

                if (enable_validation)
                {
                    device_create_info.enabledLayerCount = debug::validation_layer_names.size();
                    device_create_info.ppEnabledLayerNames = debug::validation_layer_names.data();
                }

                VK_VERIFY(vkCreateDevice(vk_gpu, &device_create_info, nullptr, &vk_device));

                // Get list of supported formats
                uint32_t format_count;
                VK_VERIFY(GetPhysicalDeviceSurfaceFormatsKHR(vk_gpu, vk_surface, &format_count, nullptr));

                std::vector<VkSurfaceFormatKHR> surface_formats(format_count);

                VK_VERIFY(
                        GetPhysicalDeviceSurfaceFormatsKHR(vk_gpu, vk_surface, &format_count, surface_formats.data()));
                VERIFY(format_count >= 1, "Failed get vulkan surface formats");

                if (format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
                {
                    vk_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else
                {
                    vk_surface_format.format = surface_formats[0].format;
                }
                vk_surface_format.colorSpace = surface_formats[0].colorSpace;

                return true;
            }

            bool enum_devices()
            {
                uint32_t gpu_count = 0;
                VK_VERIFY(vkEnumeratePhysicalDevices(vk_instance, &gpu_count, nullptr));
                VERIFY(gpu_count > 0, "There are no gpu devices");

                std::vector<VkPhysicalDevice> gpus(gpu_count);
                VK_VERIFY(vkEnumeratePhysicalDevices(vk_instance, &gpu_count, gpus.data()));

                vk_gpu = gpus[0];

                uint32_t queue_count;
                vkGetPhysicalDeviceQueueFamilyProperties(vk_gpu, &queue_count, nullptr);
                VERIFY(queue_count > 0, "There are no compatible gpu devices");

                std::vector<VkQueueFamilyProperties> queue_properties;
                queue_properties.resize(queue_count);
                vkGetPhysicalDeviceQueueFamilyProperties(vk_gpu, &queue_count, queue_properties.data());


                VkBool32 supports_present;
                for (uint32_t i = 0; i < queue_count; i++)
                {
                    if (vk_graphics_queue_index == -1 && queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        vk_graphics_queue_index = i;
                    }

                    if (vk_present_queue_index == -1)
                    {
                        GetPhysicalDeviceSurfaceSupportKHR(vk_gpu, i, vk_surface, &supports_present);
                        if (supports_present == VK_TRUE)
                        {
                            vk_present_queue_index = i;
                        }
                    }
                }

                VERIFY(vk_graphics_queue_index < queue_count, "There are no vulkan graphics queues");
                VERIFY(vk_present_queue_index < queue_count, "There are no vulkan present queues");
                VERIFY(vk_present_queue_index == vk_present_queue_index, "Vulkan graphics and present queues not equals");

                if (!create_device(vk_graphics_queue_index, enable_validation))
                {
                    return false;
                }

                vkGetPhysicalDeviceMemoryProperties(vk_gpu, &vk_device_memory_properties);
                vkGetDeviceQueue(vk_device, vk_graphics_queue_index, 0, &vk_queue);

                return true;
            }

            bool init(core::c_window* window, bool validation)
            {
                enable_validation = validation;

                if (!create_instance("appname"))
                {
                    return false;
                }

                if (!create_surface(window))
                {
                    return false;
                }

                if (!enum_devices())
                {
                    return false;
                }

                if (!resources::init())
                {
                    return false;
                }

                if (!swapchain::init(window))
                {
                    return false;
                }

                return true;
            }

            void shutdown()
            {
                if (vk_instance == nullptr)
                {
                    return;
                }

                swapchain::shutdown();
                resources::shutdown();

                if (vk_device != nullptr) {
                    vkDestroyDevice(vk_device, nullptr);
                }

                if (enable_validation)
                {
                    debug::shutdown();
                }

                vkDestroyInstance(vk_instance, nullptr);

                vk_gpu = nullptr;
                vk_device = nullptr;
                vk_queue = nullptr;
                vk_instance = nullptr;
            }
        };
    }
}