//
// Created by alexa on 022 22 02 2016.
//

#include <SDL2/SDL_syswm.h>
#include "render3d_globals.h"
#include "render3d_swapchain.h"
#include "render3d_utils.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace vk_globals
            {
                namespace swapchain_info
                {
                    VkFormat format = VK_FORMAT_D24_UNORM_S8_UINT;
                    VkSwapchainKHR swapchain = nullptr;
                    glm::ivec2 image_size;
                    std::vector<s_swapchain_image_info> image_buffer;
                    uint32_t current_buffer;
                }

                namespace depth_buffer_info
                {
                    VkImage image = nullptr;
                    VkImageView image_view = nullptr;
                    VkFormat format = VkFormat::VK_FORMAT_UNDEFINED;
                }
            }

            namespace swapchain
            {
                DEFINE_VK_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
                DEFINE_VK_PROC(GetPhysicalDeviceSurfacePresentModesKHR);
                DEFINE_VK_PROC(CreateSwapchainKHR);
                DEFINE_VK_PROC(DestroySwapchainKHR);
                DEFINE_VK_PROC(GetSwapchainImagesKHR);
                DEFINE_VK_PROC(AcquireNextImageKHR);
                DEFINE_VK_PROC(QueuePresentKHR);

                bool init_procs()
                {
                    GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
                    GET_INSTANCE_PROC_ADDR(vk_globals::instance, GetPhysicalDeviceSurfacePresentModesKHR);
                    GET_DEVICE_PROC_ADDR(vk_globals::device, CreateSwapchainKHR);
                    GET_DEVICE_PROC_ADDR(vk_globals::device, DestroySwapchainKHR);
                    GET_DEVICE_PROC_ADDR(vk_globals::device, GetSwapchainImagesKHR);
                    GET_DEVICE_PROC_ADDR(vk_globals::device, AcquireNextImageKHR);
                    GET_DEVICE_PROC_ADDR(vk_globals::device, QueuePresentKHR);
                    return true;
                }

                bool init_depth_buffer()
                {
                    VERIFY_LOG(utils::get_supported_depth_format(vk_globals::gpu, &(vk_globals::depth_buffer_info::format)),
                               "GPU no supports required depth format");

                    VkImageCreateInfo image_info = {};

                    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    image_info.pNext = nullptr;
                    image_info.imageType = VK_IMAGE_TYPE_2D;
                    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
                    image_info.format = vk_globals::depth_buffer_info::format;
                    image_info.extent.width = vk_globals::swapchain_info::image_size.x;
                    image_info.extent.height = vk_globals::swapchain_info::image_size.y;
                    image_info.extent.depth = 1;
                    image_info.mipLevels = 1;
                    image_info.arrayLayers = 1;
                    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
                    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
                    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    image_info.queueFamilyIndexCount = 0;
                    image_info.pQueueFamilyIndices = nullptr;
                    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    image_info.flags = 0;


                    VkImageViewCreateInfo view_info = {};
                    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    view_info.pNext = nullptr;
                    view_info.image = VK_NULL_HANDLE;
                    view_info.format = vk_globals::swapchain_info::format;
                    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
                    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
                    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
                    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
                    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    view_info.subresourceRange.baseMipLevel = 0;
                    view_info.subresourceRange.levelCount = 1;
                    view_info.subresourceRange.baseArrayLayer = 0;
                    view_info.subresourceRange.layerCount = 1;
                    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    view_info.flags = 0;

                    if (vk_globals::depth_buffer_info::format == VK_FORMAT_D16_UNORM_S8_UINT ||
                            vk_globals::depth_buffer_info::format == VK_FORMAT_D24_UNORM_S8_UINT ||
                            vk_globals::depth_buffer_info::format == VK_FORMAT_D32_SFLOAT_S8_UINT)
                    {
                        view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                    }

                    VK_VERIFY(vkCreateImage(vk_globals::device, &image_info, nullptr, &(vk_globals::depth_buffer_info::image)));

                    VkMemoryRequirements mem_reqs;
                    vkGetImageMemoryRequirements(vk_globals::device, vk_globals::depth_buffer_info::image, &mem_reqs);

                    VkMemoryAllocateInfo mem_alloc = {};
                    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    mem_alloc.pNext = nullptr;
                    mem_alloc.allocationSize = 0;
                    mem_alloc.memoryTypeIndex = 0;
                    mem_alloc.allocationSize = mem_reqs.size;

                    mem_alloc.memoryTypeIndex = utils::get_memory_type(mem_reqs.memoryTypeBits, 0);
                    VERIFY_LOG(mem_alloc.memoryTypeIndex >= 0, "There is no requeired memory type");

                    VkDeviceMemory device_memory;

                    VK_VERIFY(vkAllocateMemory(vk_globals::device, &mem_alloc, nullptr, &device_memory));
                    VK_VERIFY(vkBindImageMemory(vk_globals::device, vk_globals::depth_buffer_info::image, device_memory, 0));

                    utils::set_image_layout(vk_globals::setup_command_buffer, vk_globals::depth_buffer_info::image,
                                     view_info.subresourceRange.aspectMask,
                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

                    view_info.image = vk_globals::depth_buffer_info::image;
                    VK_VERIFY(vkCreateImageView(vk_globals::device, &view_info, nullptr, &(vk_globals::depth_buffer_info::image_view)));

                    return true;
                }

                bool init_swap_chain(core::c_window* window)
                {
                    VkSwapchainKHR old_swapchain = vk_globals::swapchain_info::swapchain;

                    // Get physical device surface properties and formats
                    VkSurfaceCapabilitiesKHR surface_caps;
                    VK_VERIFY(GetPhysicalDeviceSurfaceCapabilitiesKHR(vk_globals::gpu, vk_globals::surface, &surface_caps));

                    uint32_t present_mode_count;
                    VK_VERIFY(GetPhysicalDeviceSurfacePresentModesKHR(vk_globals::gpu, vk_globals::surface, &present_mode_count, nullptr));

                    // todo : replace with vector?
                    std::vector<VkPresentModeKHR> present_modes(present_mode_count);

                    VK_VERIFY(GetPhysicalDeviceSurfacePresentModesKHR(vk_globals::gpu, vk_globals::surface, &present_mode_count,
                                                                      present_modes.data()));

                    VkExtent2D swapchain_extent = {};
                    // width and height are either both -1, or both not -1.
                    if (surface_caps.currentExtent.width == -1)
                    {
                        vk_globals::swapchain_info::image_size = window->get_size();
                        swapchain_extent.width = vk_globals::swapchain_info::image_size.x;
                        swapchain_extent.height = vk_globals::swapchain_info::image_size.y;
                    }
                    else
                    {
                        swapchain_extent = surface_caps.currentExtent;
                    }

                    vk_globals::swapchain_info::image_size.x = swapchain_extent.width;
                    vk_globals::swapchain_info::image_size.y = swapchain_extent.height;

                    VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
                    for (size_t i = 0; i < present_mode_count; i++)
                    {
                        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                        {
                            swapchain_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
                            break;
                        }

                        if ((swapchain_present_mode != VK_PRESENT_MODE_MAILBOX_KHR) && (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
                        {
                            swapchain_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                        }
                    }

                    // Determine the number of images
                    uint32_t desired_mumber_of_swapchain_images = surface_caps.minImageCount + 1;
                    if ((surface_caps.maxImageCount > 0) && (desired_mumber_of_swapchain_images > surface_caps.maxImageCount))
                    {
                        desired_mumber_of_swapchain_images = surface_caps.maxImageCount;
                    }

                    VkSurfaceTransformFlagsKHR transform_flags;
                    if (surface_caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                    {
                        transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                    }
                    else {
                        transform_flags = surface_caps.currentTransform;
                    }

                    VkSwapchainCreateInfoKHR swapchain_create_info = {};
                    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                    swapchain_create_info.pNext = nullptr;
                    swapchain_create_info.surface = vk_globals::surface;
                    swapchain_create_info.minImageCount = desired_mumber_of_swapchain_images;
                    swapchain_create_info.imageFormat = vk_globals::surface_format.format;
                    swapchain_create_info.imageColorSpace = vk_globals::surface_format.colorSpace;
                    swapchain_create_info.imageExtent = {swapchain_extent.width, swapchain_extent.height };
                    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                    swapchain_create_info.preTransform = (VkSurfaceTransformFlagBitsKHR) transform_flags;
                    swapchain_create_info.imageArrayLayers = 1;
                    swapchain_create_info.queueFamilyIndexCount = VK_SHARING_MODE_EXCLUSIVE;
                    swapchain_create_info.queueFamilyIndexCount = 0;
                    swapchain_create_info.pQueueFamilyIndices = nullptr;
                    swapchain_create_info.presentMode = swapchain_present_mode;
                    swapchain_create_info.oldSwapchain = old_swapchain;
                    swapchain_create_info.clipped = true;
                    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

                    VK_VERIFY(CreateSwapchainKHR(vk_globals::device, &swapchain_create_info, nullptr,
                                                 &(vk_globals::swapchain_info::swapchain)));

                    if (old_swapchain != VK_NULL_HANDLE)
                    {
                        DestroySwapchainKHR(vk_globals::device, old_swapchain, nullptr);
                    }

                    uint32_t image_count;
                    VK_VERIFY(GetSwapchainImagesKHR(vk_globals::device, vk_globals::swapchain_info::swapchain,
                                                    &image_count, nullptr));

                    std::vector<VkImage> images(image_count);

                    VK_VERIFY(GetSwapchainImagesKHR(vk_globals::device, vk_globals::swapchain_info::swapchain,
                                                    &image_count, images.data()));

                    vk_globals::swapchain_info::image_buffer.resize(image_count);
                    for (uint32_t i = 0; i < image_count; i++)
                    {
                        auto& buffer = vk_globals::swapchain_info::image_buffer[i];
                        buffer.image = images[i];

                        VkImageViewCreateInfo image_view_create_info = {};
                        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                        image_view_create_info.pNext = nullptr;
                        image_view_create_info.format = vk_globals::surface_format.format;
                        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
                        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
                        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
                        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;
                        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        image_view_create_info.subresourceRange.baseMipLevel = 0;
                        image_view_create_info.subresourceRange.levelCount = 1;
                        image_view_create_info.subresourceRange.baseArrayLayer = 0;
                        image_view_create_info.subresourceRange.layerCount = 1;
                        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                        image_view_create_info.flags = 0;
                        image_view_create_info.image = buffer.image;

                        utils::set_image_layout(vk_globals::setup_command_buffer, buffer.image,
                                                VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                        VK_VERIFY(vkCreateImageView(vk_globals::device, &image_view_create_info, nullptr, &buffer.view));
                    }

                    return true;
                }

                bool init(core::c_window* window)
                {
                    VERIFY(init_procs());
                    VERIFY(init_swap_chain(window));
                    VERIFY(init_depth_buffer());

                    return true;
                }

                void shutdown()
                {
                    vkDestroyImageView(vk_globals::device, vk_globals::depth_buffer_info::image_view, nullptr);
                    vkDestroyImage(vk_globals::device, vk_globals::depth_buffer_info::image, nullptr);

                    for (auto& b : vk_globals::swapchain_info::image_buffer) {
                        vkDestroyImageView(vk_globals::device, b.view, nullptr);
                        vkDestroyImage(vk_globals::device, b.image, nullptr);
                    }
                    vkDestroySwapchainKHR(vk_globals::device, vk_globals::swapchain_info::swapchain, nullptr);
                    vk_globals::swapchain_info::image_buffer.clear();
                }

                VkResult acquire_next_image(VkSemaphore semaphore)
                {
                    return vkAcquireNextImageKHR(vk_globals::device, vk_globals::swapchain_info::swapchain, UINT64_MAX,
                                                 semaphore, (VkFence)nullptr, &(vk_globals::swapchain_info::current_buffer));
                }

                VkResult present(VkQueue queue, VkSemaphore semaphore)
                {
                    VkPresentInfoKHR present_info = {};
                    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                    present_info.pNext = nullptr;
                    present_info.swapchainCount = 1;
                    present_info.pSwapchains = &(vk_globals::swapchain_info::swapchain);
                    present_info.pImageIndices = &(vk_globals::swapchain_info::current_buffer);
                    if (semaphore != VK_NULL_HANDLE)
                    {
                        present_info.pWaitSemaphores = &semaphore;
                        present_info.waitSemaphoreCount = 1;
                    }
                    return vkQueuePresentKHR(queue, &present_info);
                }
            }
        }
    }
}