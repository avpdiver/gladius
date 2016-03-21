//
// Created by pav on 18.02.16.
//


#include <SDL2/SDL_syswm.h>
#include "render3d.h"
#include "render3d_globals.h"
#include "render3d_swapchain.h"
#include "render3d_command_buffer.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceSupportKHR);
            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceFormatsKHR);

            bool enable_validation = false;

            namespace vk_globals
            {
                bool is_init = false;
                VkInstance instance = nullptr;
                VkSurfaceKHR surface = nullptr;
                VkPhysicalDevice gpu = nullptr;
                VkDevice device = nullptr;
                VkQueue queue = nullptr;
                VkCommandBuffer setup_command_buffer = nullptr;
                uint32_t graphics_queue_index = -1;
                VkSurfaceFormatKHR surface_format = {};
                VkPhysicalDeviceMemoryProperties gpu_memory_properties = {};
                s_sync semaphores = {};

                thread_local s_thread_context thread_context = {};
            }

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
                application_info.apiVersion = VK_MAKE_VERSION(1, 0, 2); //VK_API_VERSION


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

                VK_VERIFY(vkCreateInstance(&instance_create_info, nullptr, &(vk_globals::instance)));

                GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceSupportKHR);
                GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceFormatsKHR);

                return true;
            }

            bool create_surface(core::c_window* window)
            {
#ifdef PLATFORM_WINDOWS
                VkWin32SurfaceCreateInfoKHR surface_create_info = {};
                surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                surface_create_info.hinstance = GetModuleHandle(nullptr);
                surface_create_info.hwnd = window->get_system_info()->info.win.window;
                VK_VERIFY(vkCreateWin32SurfaceKHR(vk_globals::instance, &surface_create_info, nullptr, &(vk_globals::surface)));
#endif

#if defined (PLATFORM_LINUX) || defined(PLATFORM_MACOS)
                const SDL_SysWMinfo* window_sys_info = window->get_system_info();
                VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
		        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		        surfaceCreateInfo.window = window_sys_info->info.x11.window;
		        surfaceCreateInfo.dpy = window_sys_info->info.x11.display;
		        // VK_VERIFY(vkCreateXlibSurfaceKHR(vk_globals::instance, &surfaceCreateInfo, nullptr, &(vk_globals::surface)));
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

                VK_VERIFY(vkCreateDevice(vk_globals::gpu, &device_create_info, nullptr, &(vk_globals::device)));
                vkGetPhysicalDeviceMemoryProperties(vk_globals::gpu, &(vk_globals::gpu_memory_properties));

                // Get list of supported formats
                uint32_t format_count;
                VK_VERIFY(GetPhysicalDeviceSurfaceFormatsKHR(vk_globals::gpu, vk_globals::surface, &format_count, nullptr));

                std::vector<VkSurfaceFormatKHR> surface_formats(format_count);

                VK_VERIFY(GetPhysicalDeviceSurfaceFormatsKHR(vk_globals::gpu, vk_globals::surface, &format_count,
                                                           surface_formats.data()));

                VERIFY_LOG(format_count >= 1, "Failed get vulkan surface formats");

                if (format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
                {
                    vk_globals::surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else
                {
                    vk_globals::surface_format.format = surface_formats[0].format;
                }

                vk_globals::surface_format.colorSpace = surface_formats[0].colorSpace;

                return true;
            }

            bool enum_devices()
            {
                uint32_t gpu_count = 0;
                VK_VERIFY(vkEnumeratePhysicalDevices(vk_globals::instance, &gpu_count, nullptr));
                VERIFY_LOG(gpu_count > 0, "There are no gpu devices");

                std::vector<VkPhysicalDevice> gpus(gpu_count);
                VK_VERIFY(vkEnumeratePhysicalDevices(vk_globals::instance, &gpu_count, gpus.data()));

                vk_globals::gpu = gpus[0];

                uint32_t queue_count;
                vkGetPhysicalDeviceQueueFamilyProperties(vk_globals::gpu, &queue_count, nullptr);
                VERIFY_LOG(queue_count > 0, "There are no compatible gpu devices");

                std::vector<VkQueueFamilyProperties> queue_properties;
                queue_properties.resize(queue_count);
                vkGetPhysicalDeviceQueueFamilyProperties(vk_globals::gpu, &queue_count, queue_properties.data());

                VkBool32 supports_present;
                for (uint32_t i = 0; i < queue_count; i++)
                {
                    if (vk_globals::graphics_queue_index == -1 &&
                        queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    {
                        GetPhysicalDeviceSurfaceSupportKHR(vk_globals::gpu, i, vk_globals::surface, &supports_present);
                        if (supports_present == VK_FALSE)
                        {
                            continue;
                        }

                        vk_globals::graphics_queue_index = i;
                    }
                }

                VERIFY_LOG(vk_globals::graphics_queue_index < queue_count, "There are no vulkan graphics queues");

                if (!create_device(vk_globals::graphics_queue_index, enable_validation))
                {
                    return false;
                }

                vkGetDeviceQueue(vk_globals::device, vk_globals::graphics_queue_index, 0, &(vk_globals::queue)); // TO DO Create queue later as thread-local???

                return true;
            }

            bool create_semaphores()
            {
                VkSemaphoreCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                create_info.pNext = NULL;

                // This semaphore ensures that the image is complete
                // before starting to submit again
                VK_VERIFY(vkCreateSemaphore(vk_globals::device, &create_info, nullptr,
                                            &(vk_globals::semaphores.present_semaphore)));

                // This semaphore ensures that all commands submitted
                // have been finished before submitting the image to the queue
                VK_VERIFY(vkCreateSemaphore(vk_globals::device, &create_info, nullptr,
                                            &(vk_globals::semaphores.present_semaphore)));

                return true;
            }

            bool create_setup_command_buffer()
            {
                vk_globals::setup_command_buffer = resources::create_command_buffer();
                VERIFY_LOG(vk_globals::setup_command_buffer != nullptr, "Failed create main thread command buffer");
                VERIFY(resources::begin_command_buffer(vk_globals::setup_command_buffer));
                return true;
            }

            bool init(core::c_window* window, bool validation)
            {
                enable_validation = validation;

                VERIFY(create_instance("appname"));
                VERIFY(create_surface(window));
                VERIFY(enum_devices());
                VERIFY(create_semaphores());

                // RESOURCES
                VERIFY(resources::create_command_pool());
                VERIFY(create_setup_command_buffer());

                VERIFY(swapchain::init(window));

                VERIFY(resources::flush_command_buffer(vk_globals::queue, vk_globals::setup_command_buffer));

                resources::destroy(vk_globals::setup_command_buffer);

                vk_globals::is_init = true;

                return true;
            }

            void shutdown()
            {
                if (vk_globals::instance == nullptr)
                {
                    return;
                }

                swapchain::shutdown();

                if (vk_globals::thread_context.command_pool != nullptr)
                {
                    vkDestroyCommandPool(vk_globals::device, vk_globals::thread_context.command_pool, nullptr);
                }

                if (vk_globals::device != nullptr) {
                    vkDestroyDevice(vk_globals::device, nullptr);
                }

                if (enable_validation)
                {
                    debug::shutdown();
                }

                vkDestroyInstance(vk_globals::instance, nullptr);

                vk_globals::gpu = nullptr;
                vk_globals::device = nullptr;
                vk_globals::queue = nullptr;
                vk_globals::instance = nullptr;
            }

            bool render()
            {
                if (!vk_globals::is_init)
                {
                    return false;
                }

                vkDeviceWaitIdle(vk_globals::device);

                VK_VERIFY(swapchain::acquire_next_image(vk_globals::semaphores.present_semaphore));
                VK_VERIFY(swapchain::present(vk_globals::queue, vk_globals::semaphores.render_semaphores));

                // draw();
                vkDeviceWaitIdle(vk_globals::device);

                return true;
            }
        };
    }
}