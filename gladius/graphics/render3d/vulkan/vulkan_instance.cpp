//
// Created by alexa on 029 29 12 2016.
//

#include "vulkan_instance.h"
#include "vulkan_macros.h"
#include "vulkan_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool create_instance(c_renderer3d* renderer, const char *app_name) {
    uint32_t extensions_count = 0;
    VK_VERIFY (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr));
    VERIFY_LOG (extensions_count > 0, LOG_TYPE, "Error occurred during instance extensions enumeration!", "");

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    VK_VERIFY (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, &available_extensions[0]));

    std::vector<const char *> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef PLATFORM_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif PLATFORM_LINUX
            VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
    };

    for (size_t i = 0; i < extensions.size(); ++i) {
        VERIFY_LOG(utils::check_extension(extensions[i], available_extensions), LOG_TYPE,
                   "Could not find instance extension named \"%s\"!", extensions[i]);
    }

    VkApplicationInfo application_info = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType
            nullptr,                                        // const void                *pNext
            app_name,                                       // const char                *pApplicationName
            VK_MAKE_VERSION(0, 0, 1),                       // uint32_t                   applicationVersion
            "gladius",                                      // const char                *pEngineName
            VK_MAKE_VERSION(0, 0, 1),                       // uint32_t                   engineVersion
            VK_MAKE_VERSION(1, 0, 21)                       // uint32_t                   apiVersion
    };

    VkInstanceCreateInfo instance_create_info = {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
            nullptr,                                        // const void                *pNext
            0,                                              // VkInstanceCreateFlags      flags
            &application_info,                              // const VkApplicationInfo   *pApplicationInfo
            0,                                              // uint32_t                   enabledLayerCount
            nullptr,                                        // const char * const        *ppEnabledLayerNames
            static_cast<uint32_t>(extensions.size()),      // uint32_t                   enabledExtensionCount
            &extensions[0]                                  // const char * const        *ppEnabledExtensionNames
    };

    VK_VERIFY(vkCreateInstance(&instance_create_info, nullptr, &(renderer->m_instance)));

    return true;
}

}
}
}
