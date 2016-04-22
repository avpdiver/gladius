//
// Created by pav on 18.02.16.
//

#include "render3d.h"
#include "render3d_globals.h"
#include "render3d_command_buffer.h"
#include "render3d_utils.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            size_t on_window_close = static_cast<size_t >(-1);

            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceSupportKHR);
            DEFINE_VK_PROC(GetPhysicalDeviceSurfaceFormatsKHR);

            namespace vk_globals
            {
                bool is_init = false;
                VkInstance instance = nullptr;
                VkSurfaceKHR surface = nullptr;
                VkPhysicalDevice gpu = nullptr;
                VkDevice device = nullptr;

                VkCommandBuffer setup_command_buffer = nullptr;

                s_device_queue graphics_queue = {};
                s_device_queue present_queue = {};

                s_swapchain_info swapchain_info = {};

                VkSurfaceFormatKHR surface_format = {};
                VkPhysicalDeviceMemoryProperties gpu_memory_properties = {};
                s_sync_semaphores semaphores = {};

                thread_local s_thread_context thread_context = {};
            }

            bool create_instance(const char* app_name)
            {
                uint32_t extensions_count = 0;
                VK_VERIFY (vkEnumerateInstanceExtensionProperties (nullptr, &extensions_count, nullptr));
                VERIFY_LOG (extensions_count > 0, "Error occurred during instance extensions enumeration!");

                std::vector<VkExtensionProperties> available_extensions (extensions_count);
                VK_VERIFY (vkEnumerateInstanceExtensionProperties (nullptr, &extensions_count, &available_extensions[0]));

                std::vector<const char *> extensions = {
                    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef PLATFORM_WINDOWS
                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif PLATFORM_LINUX
                    VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
                };

                for (size_t i = 0; i < extensions.size (); ++i)
                {
                    VERIFY_LOGF(utils::check_extension (extensions[i], available_extensions),
                                "Could not find instance extension named \"%s\"!", extensions[i])
                }

                VkApplicationInfo application_info = {
                    VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType
                    nullptr,                                        // const void                *pNext
                    app_name,                                       // const char                *pApplicationName
                    VK_MAKE_VERSION(1, 0, 8),                       // uint32_t                   applicationVersion
                    "gladius",                                      // const char                *pEngineName
                    VK_MAKE_VERSION(1, 0, 8),                       // uint32_t                   engineVersion
                    VK_MAKE_VERSION(1, 0, 8)                        // uint32_t                   apiVersion
                };

                VkInstanceCreateInfo instance_create_info = {
                    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
                    nullptr,                                        // const void                *pNext
                    0,                                              // VkInstanceCreateFlags      flags
                    &application_info,                              // const VkApplicationInfo   *pApplicationInfo
                    0,                                              // uint32_t                   enabledLayerCount
                    nullptr,                                        // const char * const        *ppEnabledLayerNames
                    static_cast<uint32_t>(extensions.size ()),      // uint32_t                   enabledExtensionCount
                    &extensions[0]                                  // const char * const        *ppEnabledExtensionNames
                };

                VK_VERIFY(vkCreateInstance (&instance_create_info, nullptr, &(vk_globals::instance)));

                GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceSupportKHR);
                GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceFormatsKHR);

                return true;
            }

            bool create_surface(core::c_window* window)
            {
                const core::s_window_info *window_sys_info = window->get_system_info ();

#ifdef PLATFORM_WINDOWS
                VkWin32SurfaceCreateInfoKHR surface_create_info = {
                      VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                  sType
                      nullptr,                                          // const void                      *pNext
                      0,                                                // VkWin32SurfaceCreateFlagsKHR     flags
                      window_sys_info->instance,                        // HINSTANCE                        hinstance
                      window_sys_info->handle                           // HWND                             hwnd
                };
                VK_VERIFY (vkCreateWin32SurfaceKHR(vk_globals::instance, &surface_create_info, nullptr, &(vk_globals::surface)));

#elif PLATFORM_LINUX
                VkXcbSurfaceCreateInfoKHR surface_create_info = {
                    VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                  sType
                    nullptr,                                          // const void                      *pNext
                    0,                                                // VkXcbSurfaceCreateFlagsKHR       flags
                    window_sys_info->connection,                      // xcb_connection_t*                connection
                    window_sys_info->handle                           // xcb_window_t                     window
                };
                VK_VERIFY (vkCreateXcbSurfaceKHR (vk_globals::instance, &surface_create_info, nullptr, &(vk_globals::surface)));
#endif
                return true;
            }

            bool create_device()
            {
                uint32_t num_devices = 0;
                VK_VERIFY (vkEnumeratePhysicalDevices (vk_globals::instance, &num_devices, nullptr));
                VERIFY_LOG (num_devices > 0, "Error occurred during physical devices enumeration!");

                std::vector<VkPhysicalDevice> physical_devices (num_devices);
                VK_VERIFY(vkEnumeratePhysicalDevices (vk_globals::instance, &num_devices, &physical_devices[0]));

                uint32_t selected_graphics_queue_family_index = UINT32_MAX;
                uint32_t selected_present_queue_family_index = UINT32_MAX;

                for (uint32_t i = 0; i < num_devices; ++i)
                {
                    if (utils::check_physical_device_properties (physical_devices[i], selected_graphics_queue_family_index, selected_present_queue_family_index))
                    {
                        vk_globals::gpu = physical_devices[i];
                    }
                }

                VERIFY_LOG (vk_globals::gpu != nullptr, "Could not select physical device based on the chosen properties!");

                std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
                std::vector<float> queue_priorities = {1.0f};

                queue_create_infos.push_back (
                    {
                        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType              sType
                        nullptr,                                          // const void                  *pNext
                        0,                                                // VkDeviceQueueCreateFlags     flags
                        selected_graphics_queue_family_index,             // uint32_t                     queueFamilyIndex
                        static_cast<uint32_t>(queue_priorities.size ()),  // uint32_t                     queueCount
                        &queue_priorities[0]                              // const float                 *pQueuePriorities
                    }
                );

                if (selected_graphics_queue_family_index != selected_present_queue_family_index)
                {
                    queue_create_infos.push_back (
                        {
                            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // VkStructureType              sType
                            nullptr,                                        // const void                  *pNext
                            0,                                              // VkDeviceQueueCreateFlags     flags
                            selected_present_queue_family_index,            // uint32_t                     queueFamilyIndex
                            static_cast<uint32_t>(queue_priorities.size ()), // uint32_t                     queueCount
                            &queue_priorities[0]                            // const float                 *pQueuePriorities
                        }
                    );
                }

                std::vector<const char *> extensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };

                VkDeviceCreateInfo device_create_info = {
                    VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,             // VkStructureType                    sType
                    nullptr,                                          // const void                        *pNext
                    0,                                                // VkDeviceCreateFlags                flags
                    static_cast<uint32_t>(queue_create_infos.size ()), // uint32_t                           queueCreateInfoCount
                    &queue_create_infos[0],                           // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
                    0,                                                // uint32_t                           enabledLayerCount
                    nullptr,                                          // const char * const                *ppEnabledLayerNames
                    static_cast<uint32_t>(extensions.size ()),         // uint32_t                           enabledExtensionCount
                    &extensions[0],                                   // const char * const                *ppEnabledExtensionNames
                    nullptr                                           // const VkPhysicalDeviceFeatures    *pEnabledFeatures
                };

                VK_VERIFY (vkCreateDevice (vk_globals::gpu, &device_create_info, nullptr, &(vk_globals::device)));

                vk_globals::graphics_queue.index = selected_graphics_queue_family_index;
                vk_globals::present_queue.index = selected_present_queue_family_index;

                return true;
            }

            bool create_device_queue() {
                vkGetDeviceQueue(vk_globals::device, vk_globals::graphics_queue.index, 0, &(vk_globals::graphics_queue.queue));
                vkGetDeviceQueue(vk_globals::device, vk_globals::present_queue.index, 0, &(vk_globals::present_queue.queue));
                return true;
            }

            bool create_swap_chain()
            {
                if (vk_globals::device != nullptr)
                {
                    vkDeviceWaitIdle (vk_globals::device);
                }

                VkSurfaceCapabilitiesKHR surface_capabilities;
                VK_VERIFY (vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vk_globals::gpu, vk_globals::surface, &surface_capabilities));

                uint32_t formats_count;
                VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR (vk_globals::gpu, vk_globals::surface, &formats_count, nullptr));
                VERIFY_LOG(formats_count > 0, "Error occurred during presentation surface formats enumeration!");

                std::vector<VkSurfaceFormatKHR> surface_formats (formats_count);
                VK_VERIFY(vkGetPhysicalDeviceSurfaceFormatsKHR (vk_globals::gpu, vk_globals::surface, &formats_count, &surface_formats[0]));

                uint32_t present_modes_count;
                VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR (vk_globals::gpu, vk_globals::surface, &present_modes_count, nullptr));
                VERIFY_LOG(present_modes_count > 0, "Error occurred during presentation surface present modes enumeration!");

                std::vector<VkPresentModeKHR> present_modes (present_modes_count);
                VK_VERIFY(vkGetPhysicalDeviceSurfacePresentModesKHR (vk_globals::gpu, vk_globals::surface, &present_modes_count, &present_modes[0]));

                uint32_t desired_number_of_images = utils::get_swapchain_num_images (surface_capabilities);
                VkSurfaceFormatKHR desired_format = utils::get_swapchain_format (surface_formats);
                VkExtent2D desired_extent = utils::get_swapchain_extent (surface_capabilities);
                VkImageUsageFlags desired_usage = utils::get_swapchain_usage_flags (surface_capabilities);
                VkSurfaceTransformFlagBitsKHR desired_transform = utils::get_swapchain_transform (surface_capabilities);
                VkPresentModeKHR desired_present_mode = utils::get_swapchain_present_mode (present_modes);
                VkSwapchainKHR old_swap_chain = vk_globals::swapchain_info.swapchain;

                VERIFY (static_cast<int>(desired_usage) != -1);
                VERIFY (static_cast<int>(desired_present_mode) != -1);

                VkSwapchainCreateInfoKHR swap_chain_create_info = {
                    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
                    nullptr,                                      // const void                    *pNext
                    0,                                            // VkSwapchainCreateFlagsKHR      flags
                    vk_globals::surface,                          // VkSurfaceKHR                   surface
                    desired_number_of_images,                     // uint32_t                       minImageCount
                    desired_format.format,                        // VkFormat                       imageFormat
                    desired_format.colorSpace,                    // VkColorSpaceKHR                imageColorSpace
                    desired_extent,                               // VkExtent2D                     imageExtent
                    1,                                            // uint32_t                       imageArrayLayers
                    desired_usage,                                // VkImageUsageFlags              imageUsage
                    VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
                    0,                                            // uint32_t                       queueFamilyIndexCount
                    nullptr,                                      // const uint32_t                *pQueueFamilyIndices
                    desired_transform,                            // VkSurfaceTransformFlagBitsKHR  preTransform
                    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
                    desired_present_mode,                         // VkPresentModeKHR               presentMode
                    VK_TRUE,                                      // VkBool32                       clipped
                    old_swap_chain                                // VkSwapchainKHR                 oldSwapchain
                };

                VK_VERIFY (vkCreateSwapchainKHR (vk_globals::device, &swap_chain_create_info, nullptr, &(vk_globals::swapchain_info.swapchain)));
                if (old_swap_chain != nullptr)
                {
                    vkDestroySwapchainKHR (vk_globals::device, old_swap_chain, nullptr);
                }

                vk_globals::swapchain_info.format = desired_format.format;

                uint32_t image_count = 0;
                VK_VERIFY(vkGetSwapchainImagesKHR (vk_globals::device, vk_globals::swapchain_info.swapchain, &image_count, nullptr));
                VERIFY_LOG(image_count > 0, "Could not get swap chain images!");

                vk_globals::swapchain_info.images.resize (image_count);
                VK_VERIFY (vkGetSwapchainImagesKHR (vk_globals::device, vk_globals::swapchain_info.swapchain, &image_count, &(vk_globals::swapchain_info.images[0])));

                return true;
            }

            bool create_semaphores()
            {
                VkSemaphoreCreateInfo semaphore_create_info = {
                    VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
                    nullptr,                                      // const void*              pNext
                    0                                             // VkSemaphoreCreateFlags   flags
                };

                // This semaphore ensures that the image is complete
                // before starting to submit again
                VK_VERIFY(vkCreateSemaphore(vk_globals::device, &semaphore_create_info, nullptr,
                                            &(vk_globals::semaphores.image_available_semaphore)));

                // This semaphore ensures that all commands submitted
                // have been finished before submitting the image to the queue
                VK_VERIFY(vkCreateSemaphore(vk_globals::device, &semaphore_create_info, nullptr,
                                            &(vk_globals::semaphores.rendering_finished_semaphore)));

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
                VERIFY(create_instance ("appname"));
                VERIFY(create_surface (window));
                VERIFY(create_device ());
                VERIFY(create_device_queue ());
                VERIFY(create_swap_chain ());
                VERIFY(create_semaphores ());

                // RESOURCES
                /*VERIFY(resources::create_command_pool ());
                VERIFY(create_setup_command_buffer ());
                VERIFY(resources::flush_command_buffer (vk_globals::queue, vk_globals::setup_command_buffer));

                resources::destroy (vk_globals::setup_command_buffer);
                */

                vk_globals::is_init = true;

                on_window_close = window->add_event_listener (core::e_window_event::ON_CLOSE, [window, &on_window_close](void* ptr) {
                    window->remove_event_listener (on_window_close);
                    on_window_close = static_cast<size_t >(-1);
                    shutdown ();
                });

                return true;
            }

            void shutdown()
            {
                if (vk_globals::device != nullptr)
                {
                    vkDeviceWaitIdle (vk_globals::device);

                    if (vk_globals::semaphores.image_available_semaphore != nullptr)
                    {
                        vkDestroySemaphore (vk_globals::device, vk_globals::semaphores.image_available_semaphore, nullptr);
                    }
                    if (vk_globals::semaphores.rendering_finished_semaphore != nullptr)
                    {
                        vkDestroySemaphore (vk_globals::device, vk_globals::semaphores.rendering_finished_semaphore, nullptr);
                    }
                    if (vk_globals::swapchain_info.swapchain != nullptr)
                    {
                        vkDestroySwapchainKHR (vk_globals::device, vk_globals::swapchain_info.swapchain, nullptr);
                    }
                    vkDestroyDevice (vk_globals::device, nullptr);
                }

                if (vk_globals::surface != nullptr)
                {
                    vkDestroySurfaceKHR (vk_globals::instance, vk_globals::surface, nullptr);
                }

                if (vk_globals::instance != nullptr)
                {
                    vkDestroyInstance (vk_globals::instance, nullptr);
                }
            }

            bool render()
            {
                if (!vk_globals::is_init)
                {
                    return false;
                }

                vkDeviceWaitIdle(vk_globals::device);


                // draw();
                vkDeviceWaitIdle(vk_globals::device);

                return true;
            }
        }
    }
}