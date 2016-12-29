//
// Created by pav on 18.02.16.
//

#include <atomic>
#include "render3d.h"
#include "render3d_globals.h"
#include "render3d_device.h"
#include "render3d_command_buffer.h"
#include "render3d_utils.h"
#include "render3d_resources.h"
#include "render3d_macros.h"
#include "render3d_surface.h"
#include "render3d_swapchain.h"

namespace gladius {
namespace graphics {
namespace render3d {

size_t              on_window_resize_listener = static_cast<size_t >(-1);
std::atomic_bool    flag_needs_shutdown;
std::atomic_bool    flag_needs_recreate_swapchain;
s_swapchain_desc    swapchain_desc;

namespace vk_globals {
bool is_init = false;
VkInstance instance = nullptr;
VkSurfaceFormatKHR surface_format = {};
c_pipeline_factory pipeline_factory;
}

void on_window_resize(void *value) {
    flag_needs_recreate_swapchain.store(true);
}

bool create_instance(const char *app_name) {
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

    VK_VERIFY(vkCreateInstance(&instance_create_info, nullptr, &(vk_globals::instance)));

    return true;
}

bool init(core::c_window *window, const s_render3d_desc& desc) {
    VERIFY(create_instance("appname"));
    VERIFY(create_surface(window));
    VERIFY(create_device());
    VERIFY(create_device_queue());
    VERIFY(get_surface_capabilites());

    swapchain_desc.format = VK_FORMAT_B8G8R8A8_UNORM;
    swapchain_desc.imageCount = 2;
    VERIFY(create_swap_chain(swapchain_desc));

    vk_globals::is_init = true;
    flag_needs_recreate_swapchain.store(false);
    flag_needs_shutdown.store(false);
    on_window_resize_listener = window->add_event_listener(core::e_window_event::ON_RESIZE, on_window_resize);

    return true;
}

void shutdown() {

    on_window_resize_listener = static_cast<size_t >(-1);

    if (vk_globals::device != nullptr) {
        vkDeviceWaitIdle(vk_globals::device);
        shutdown_swap_chain();
        delete vk_globals::gpu_memory_allocator;
        vkDestroyDevice(vk_globals::device, nullptr);
        vk_globals::device = nullptr;
    }

    if (vk_globals::surface.surface != nullptr) {
        vkDestroySurfaceKHR(vk_globals::instance, vk_globals::surface.surface, nullptr);
    }

    if (vk_globals::instance != nullptr) {
        vkDestroyInstance(vk_globals::instance, nullptr);
    }
}

bool check_events() {
    bool true_value = true;
    if (flag_needs_recreate_swapchain.compare_exchange_weak(true_value, false)) {
        create_swap_chain(swapchain_desc);
    }
    return true;
}

bool fill_present_command_buffer(VkCommandBuffer command_buffer, VkImage image) {

    static VkCommandBufferBeginInfo cmd_buffer_begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
            nullptr,                                      // const void                            *pNext
            VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, // VkCommandBufferUsageFlags              flags
            nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
    };

    static VkClearColorValue clear_color = {
            {1.0f, 0.8f, 0.7f, 0.0f}
    };

    static VkImageSubresourceRange image_subresource_range = {
            VK_IMAGE_ASPECT_COLOR_BIT,                    // VkImageAspectFlags                     aspectMask
            0,                                            // uint32_t                               baseMipLevel
            1,                                            // uint32_t                               levelCount
            0,                                            // uint32_t                               baseArrayLayer
            1                                             // uint32_t                               layerCount
    };

    static VkImageMemoryBarrier barrier_from_present_to_clear = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
            nullptr,                                    // const void                            *pNext
            VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
            VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
            VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                          oldLayout
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
            vk_globals::present_queue.index,            // uint32_t                               srcQueueFamilyIndex
            vk_globals::present_queue.index,            // uint32_t                               dstQueueFamilyIndex
            image,                                      // VkImage                                image
            image_subresource_range                     // VkImageSubresourceRange                subresourceRange
    };

    static VkImageMemoryBarrier barrier_from_clear_to_present = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
            nullptr,                                    // const void                            *pNext
            VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          srcAccessMask
            VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          dstAccessMask
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          oldLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                          newLayout
            vk_globals::present_queue.index,            // uint32_t                               srcQueueFamilyIndex
            vk_globals::present_queue.index,            // uint32_t                               dstQueueFamilyIndex
            image,                                      // VkImage                                image
            image_subresource_range                     // VkImageSubresourceRange                subresourceRange
    };

    vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin_info);
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier_from_present_to_clear);

    vkCmdClearColorImage(command_buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1,
                         &image_subresource_range);

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier_from_clear_to_present);
    VK_VERIFY (vkEndCommandBuffer(command_buffer));


    return true;
}

bool render() {
    if (!vk_globals::is_init) {
        return false;
    }

    vkDeviceWaitIdle(vk_globals::device);

    if (!check_events()) {
        return false;
    }

    resources::s_render_context const* render_context;
    VERIFY(resources::get_current_render_context(&render_context));

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(vk_globals::device, vk_globals::swapchain.handle,
                                            UINT64_MAX, render_context->image_available_semaphore,
                                            VK_NULL_HANDLE, &image_index);
    switch (result) {
    case VK_SUCCESS:
        break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR:
        create_swap_chain(swapchain_desc);
        return true;
    default:
        SET_ERROR (LOG_TYPE, "Problem occurred during swap chain image acquisition!", "");
        return false;
    }

    VERIFY(fill_present_command_buffer(render_context->command_buffer, vk_globals::swapchain.images[image_index]));

    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,                                  // VkStructureType              sType
        nullptr,                                                        // const void                  *pNext
        1,                                                              // uint32_t                     waitSemaphoreCount
        &render_context->image_available_semaphore,                     // const VkSemaphore           *pWaitSemaphores
        &wait_dst_stage_mask,                                           // const VkPipelineStageFlags  *pWaitDstStageMask;
        1,                                                              // uint32_t                     commandBufferCount
        &render_context->command_buffer,                                // const VkCommandBuffer       *pCommandBuffers
        1,                                                              // uint32_t                     signalSemaphoreCount
        &render_context->rendering_finished_semaphore                   // const VkSemaphore           *pSignalSemaphores
    };

    VK_VERIFY (vkQueueSubmit(vk_globals::present_queue.handle, 1, &submit_info, render_context->fence));

    VkPresentInfoKHR present_info = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                         // VkStructureType              sType
        nullptr,                                                    // const void                  *pNext
        1,                                                          // uint32_t                     waitSemaphoreCount
        &render_context->rendering_finished_semaphore,              // const VkSemaphore           *pWaitSemaphores
        1,                                                          // uint32_t                     swapchainCount
        &(vk_globals::swapchain.handle),                           // const VkSwapchainKHR        *pSwapchains
        &image_index,                                               // const uint32_t              *pImageIndices
        nullptr                                                     // VkResult                    *pResults
    };
    result = vkQueuePresentKHR(vk_globals::present_queue.handle, &present_info);

    switch (result) {
    case VK_SUCCESS:
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
        return true;
    default:
        SET_ERROR (LOG_TYPE, "Problem occurred during image presentation!", "");
        return false;
    }

    return true;
}

}
}
}