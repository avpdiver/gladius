//
// Created by pav on 18.02.16.
//

#include "../core/logging/logging.h"

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

#include "renderer3d.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            VkInstance vk_instance = nullptr;

            bool create_instance(const char* app_name, bool enable_validation)
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
                    instance_create_info.enabledLayerCount = debug::validation_layer_count;
                    instance_create_info.ppEnabledLayerNames = debug::validation_layer_names;
                }

                VkResult result = vkCreateInstance(&instance_create_info, nullptr, &vk_instance);
                if (result != VK_SUCCESS)
                {
                    SET_ERROR("Create instance error %u", result);
                    return false;
                }

                return true;
            }

            bool init(core::c_window* window, bool enable_validation)
            {
                if (!create_instance("appname", enable_validation))
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

                debug::shutdown(vk_instance);
            }
        };
    }
}