//
// Created by alexa on 028 28 08 2016.
//

#include "render3d_globals.h"
#include "render3d_device.h"
#include "render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

namespace vk_globals {
VkPhysicalDevice gpu = nullptr;
VkDevice device = nullptr;

s_device_queue graphics_queue = {};
s_device_queue present_queue = {};
}

bool check_physical_device_properties(VkPhysicalDevice physical_device,
                                      uint32_t &selected_graphics_queue_family_index,
                                      uint32_t &selected_present_queue_family_index,
                                      const std::vector<const char *> &device_extensions) {
    uint32_t extensions_count = 0;
    if ((vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr)
        != VK_SUCCESS)
        || (extensions_count == 0)) {
        SET_ERROR ("Error occurred during physical device %d extensions enumeration!", physical_device);
        return false;
    }

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    if (vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count,
                                             &available_extensions[0]) != VK_SUCCESS) {
        SET_ERROR ("Error occurred during physical device %d extensions enumeration!", physical_device);
        return false;
    }

    for (size_t i = 0; i < device_extensions.size(); ++i) {
        if (!utils::check_extension(device_extensions[i], available_extensions)) {
            SET_ERROR ("Physical device %d doesn't support extension named \"%s\"!", physical_device,
                        device_extensions[i]);
            return false;
        }
    }

    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;

    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);

    if ((major_version < 1) && (device_properties.limits.maxImageDimension2D < 4096)) {
        SET_ERROR ("Physical device %d doesn't support required parameters!", physical_device);
        return false;
    }

    uint32_t queue_families_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
    if (queue_families_count == 0) {
        SET_ERROR ("Physical device %d doesn't have any handle families!", physical_device);
        return false;
    }

    std::vector<VkQueueFamilyProperties> queue_family_properties(queue_families_count);
    std::vector<VkBool32> queue_present_support(queue_families_count);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, &queue_family_properties[0]);

    uint32_t graphics_queue_family_index = UINT32_MAX;
    uint32_t present_queue_family_index = UINT32_MAX;

    for (uint32_t i = 0; i < queue_families_count; ++i) {
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, vk_globals::surface, &queue_present_support[i]);

        if ((queue_family_properties[i].queueCount > 0)
            && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            // Select first handle that supports graphics
            if (graphics_queue_family_index == UINT32_MAX) {
                graphics_queue_family_index = i;
            }

            // If there is handle that supports both graphics and present - prefer it
            if (queue_present_support[i]) {
                selected_graphics_queue_family_index = i;
                selected_present_queue_family_index = i;
                return true;
            }
        }
    }

    // We don't have handle that supports both graphics and present so we have to use separate queues
    for (uint32_t i = 0; i < queue_families_count; ++i) {
        if (queue_present_support[i]) {
            present_queue_family_index = i;
            break;
        }
    }

    // If this device doesn't support queues with graphics and present capabilities don't use it
    if ((graphics_queue_family_index == UINT32_MAX) || (present_queue_family_index == UINT32_MAX)) {
        SET_ERROR ("Could not find handle families with required properties on physical device %d!",
                    physical_device);
        return false;
    }

    selected_graphics_queue_family_index = graphics_queue_family_index;
    selected_present_queue_family_index = present_queue_family_index;
    return true;
}

bool create_device() {
    uint32_t num_devices = 0;
    VK_VERIFY (vkEnumeratePhysicalDevices(vk_globals::instance, &num_devices, nullptr));
    VERIFY_LOG (num_devices > 0, "Error occurred during physical devices enumeration!", "");

    std::vector<VkPhysicalDevice> physical_devices(num_devices);
    VK_VERIFY(vkEnumeratePhysicalDevices(vk_globals::instance, &num_devices, &physical_devices[0]));

    uint32_t selected_graphics_queue_family_index = UINT32_MAX;
    uint32_t selected_present_queue_family_index = UINT32_MAX;
    std::vector<const char *> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (uint32_t i = 0; i < num_devices; ++i) {
        if (check_physical_device_properties(physical_devices[i],
                                             selected_graphics_queue_family_index,
                                             selected_present_queue_family_index,
                                             extensions)) {
            vk_globals::gpu = physical_devices[i];
        }
    }

    VERIFY_LOG (vk_globals::gpu != nullptr,
                "Could not select physical device based on the chosen properties!", "");

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<float> queue_priorities = {1.0f};

    queue_create_infos.push_back(
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType              sType
            nullptr,                                          // const void                  *pNext
            0,                                                // VkDeviceQueueCreateFlags     flags
            selected_graphics_queue_family_index,             // uint32_t                     queueFamilyIndex
            static_cast<uint32_t>(queue_priorities.size()),  // uint32_t                     queueCount
            &queue_priorities[0]                              // const float                 *pQueuePriorities
        }
    );

    if (selected_graphics_queue_family_index != selected_present_queue_family_index) {
        queue_create_infos.push_back(
            {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // VkStructureType              sType
                nullptr,                                        // const void                  *pNext
                0,                                              // VkDeviceQueueCreateFlags     flags
                selected_present_queue_family_index,            // uint32_t                     queueFamilyIndex
                static_cast<uint32_t>(queue_priorities.size()), // uint32_t                     queueCount
                &queue_priorities[0]                            // const float                 *pQueuePriorities
            }
        );
    }

    VkDeviceCreateInfo device_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,             // VkStructureType                    sType
        nullptr,                                          // const void                        *pNext
        0,                                                // VkDeviceCreateFlags                flags
        static_cast<uint32_t>(queue_create_infos.size()), // uint32_t                           queueCreateInfoCount
        &queue_create_infos[0],                           // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
        0,                                                // uint32_t                           enabledLayerCount
        nullptr,                                          // const char * const                *ppEnabledLayerNames
        static_cast<uint32_t>(extensions.size()),         // uint32_t                           enabledExtensionCount
        &extensions[0],                                   // const char * const                *ppEnabledExtensionNames
        nullptr                                           // const VkPhysicalDeviceFeatures    *pEnabledFeatures
    };

    VK_VERIFY (vkCreateDevice(vk_globals::gpu, &device_create_info, nullptr, &(vk_globals::device)));

    vk_globals::graphics_queue.index = selected_graphics_queue_family_index;
    vk_globals::present_queue.index = selected_present_queue_family_index;

    return true;
}

bool create_device_queue() {
    vkGetDeviceQueue(vk_globals::device, vk_globals::graphics_queue.index, 0, &(vk_globals::graphics_queue.handle));
    vkGetDeviceQueue(vk_globals::device, vk_globals::present_queue.index, 0, &(vk_globals::present_queue.handle));
    return true;
}

}
}
}