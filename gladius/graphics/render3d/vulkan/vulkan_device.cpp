//
// Created by alexa on 028 28 08 2016.
//

#include "vulkan_device.h"
#include "vulkan_utils.h"
#include "vulkan_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool check_gpu_properties(c_renderer3d* renderer, const std::vector<const char *>& required_extensions,
                          const std::vector<VkQueueFlagBits>& required_queues) {

    vkGetPhysicalDeviceProperties(renderer->m_gpu, &renderer->m_device_properties);
    vkGetPhysicalDeviceFeatures(renderer->m_gpu, &renderer->m_device_features);

    VERIFY_LOG(((VK_VERSION_MAJOR(renderer->m_device_properties.apiVersion) >= 1) && (renderer->m_device_properties.limits.maxImageDimension2D >= 4096)),
               LOG_TYPE, "GPU %d doesn't support required parameters!", renderer->m_gpu);

    uint32_t extensions_count = 0;
    VK_VERIFY (vkEnumerateDeviceExtensionProperties(renderer->m_gpu, nullptr, &extensions_count, nullptr));
    VERIFY_LOG(extensions_count > 0, LOG_TYPE, "Error occurred during physical device %d extensions enumeration!", renderer->m_gpu);

    renderer->m_extensions.resize(extensions_count);
    VK_VERIFY(vkEnumerateDeviceExtensionProperties(renderer->m_gpu, nullptr, &extensions_count, renderer->m_extensions.data()));

    for (size_t i = 0; i < required_extensions.size(); ++i) {
        VERIFY_LOG(utils::check_extension(required_extensions[i], renderer->m_extensions), LOG_TYPE, 
                   "GPU %d doesn't support extension named \"%s\"!", renderer->m_gpu,
                   required_extensions[i]);
    }

    uint32_t queue_families_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(renderer->m_gpu, &queue_families_count, nullptr);
    VERIFY_LOG(queue_families_count > 0, LOG_TYPE, "GPU %d doesn't have any handle families!", renderer->m_gpu);

    renderer->m_queue_families_properties.resize(queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(renderer->m_gpu, &queue_families_count, renderer->m_queue_families_properties.data());

    for (const auto& f : required_queues) {
        uint32_t index = utils::get_queue_family_index(f, renderer->m_queue_families_properties);
        VERIFY_LOG((index != UINT32_MAX), LOG_TYPE, "GPU doesn't have queue %d", f);

        if (f & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) {
            renderer->m_graphics_queue.index = index;
        }
    }

    return true;
}

bool create_device(c_renderer3d* renderer) {
    uint32_t num_devices = 0;
    VK_VERIFY (vkEnumeratePhysicalDevices(renderer->m_instance, &num_devices, nullptr));
    VERIFY_LOG (num_devices > 0, LOG_TYPE, "Error occurred during physical devices enumeration!", "");

    std::vector<VkPhysicalDevice> physical_devices(num_devices);
    VK_VERIFY(vkEnumeratePhysicalDevices(renderer->m_instance, &num_devices, &physical_devices[0]));

    std::vector<const char *> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<VkQueueFlagBits> required_queues = {
            VK_QUEUE_GRAPHICS_BIT
    };

    for (uint32_t i = 0; i < num_devices; ++i) {
        if (check_gpu_properties(renderer, required_extensions, required_queues)) {
            renderer->m_gpu = physical_devices[i];
        }
    }

    VERIFY_LOG (renderer->m_gpu != nullptr, LOG_TYPE, "Could not select GPU based on the chosen properties!", "");

    vkGetPhysicalDeviceMemoryProperties(renderer->m_gpu, &renderer->m_gpu_memory_properties);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<float> queue_priorities = {1.0f};

    queue_create_infos.push_back(
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType              sType
            nullptr,                                          // const void                  *pNext
            0,                                                // VkDeviceQueueCreateFlags     flags
            renderer->m_graphics_queue.index,                 // uint32_t                     queueFamilyIndex
            static_cast<uint32_t>(queue_priorities.size()),   // uint32_t                     queueCount
            queue_priorities.data()                           // const float                 *pQueuePriorities
        }
    );

    VkDeviceCreateInfo device_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        static_cast<uint32_t>(queue_create_infos.size()), queue_create_infos.data(),
        0, nullptr,
        static_cast<uint32_t>(required_extensions.size()), required_extensions.data(),
        nullptr
    };

    VK_VERIFY (vkCreateDevice(renderer->m_gpu, &device_create_info, nullptr, &(renderer->m_device)));
    vkGetDeviceQueue(renderer->m_device, renderer->m_graphics_queue.index, 0, &(renderer->m_graphics_queue.handle));

    renderer->m_gpu_memory_allocator = new memory::c_gpu_memory_allocator(renderer->m_device, 268435456);

    return true;
}

}
}
}