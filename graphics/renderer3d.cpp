//
// Created by pav on 18.02.16.
//


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
            VkPhysicalDevice vk_gpu = nullptr;
            VkDevice vk_device = nullptr;
            VkQueue vk_queue = nullptr;
            VkPhysicalDeviceMemoryProperties vk_device_memory_properties;

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

                return true;
            }

            bool create_device(uint32_t queue_index, bool enable_validation) {

                std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

                float queue_priorities[] = { 0.0f };
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

                if (extensions.size() > 0) {
                    device_create_info.enabledExtensionCount = (uint32_t) extensions.size();
                    device_create_info.ppEnabledExtensionNames = extensions.data();
                }

                if (enable_validation) {
                    device_create_info.enabledLayerCount = debug::validation_layer_names.size();
                    device_create_info.ppEnabledLayerNames = debug::validation_layer_names.data();
                }

                VK_VERIFY(vkCreateDevice(vk_gpu, &device_create_info, nullptr, &vk_device));

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

                uint32_t graphics_queue_index = 0;
                uint32_t queue_count;
                vkGetPhysicalDeviceQueueFamilyProperties(vk_gpu, &queue_count, nullptr);
                VERIFY(queue_count > 0, "There are no compatible gpu devices");

                std::vector<VkQueueFamilyProperties> queue_properties;
                queue_properties.resize(queue_count);
                vkGetPhysicalDeviceQueueFamilyProperties(vk_gpu, &queue_count, queue_properties.data());
                for (const auto& p : queue_properties)
                {
                    if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        break;
                    }
                }
                VERIFY(graphics_queue_index < queue_count, "There are no compatible gpu devices");

                if (!create_device(graphics_queue_index, enable_validation))
                {
                    return false;
                }

                vkGetPhysicalDeviceMemoryProperties(vk_gpu, &vk_device_memory_properties);
                vkGetDeviceQueue(vk_device, graphics_queue_index, 0, &vk_queue);

                return true;
            }

            bool init(core::c_window* window, bool validation)
            {
                enable_validation = validation;

                if (!create_instance("appname"))
                {
                    return false;
                }

                if (!enum_devices())
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