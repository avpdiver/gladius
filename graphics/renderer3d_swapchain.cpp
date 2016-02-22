//
// Created by alexa on 022 22 02 2016.
//

#include <SDL2/SDL_syswm.h>
#include "renderer3d_common.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            namespace swapchain
            {
                struct s_swapchain_image_info
                {
                    VkImage image;
                    VkImageView view;
                };

                DEFINE_VK_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
                DEFINE_VK_PROC(GetPhysicalDeviceSurfacePresentModesKHR);
                DEFINE_VK_PROC(CreateSwapchainKHR);
                DEFINE_VK_PROC(DestroySwapchainKHR);
                DEFINE_VK_PROC(GetSwapchainImagesKHR);
                DEFINE_VK_PROC(AcquireNextImageKHR);
                DEFINE_VK_PROC(QueuePresentKHR);


                VkSwapchainKHR  vk_swapchain = nullptr;
                uint32_t vk_image_count = 0;
                std::vector<VkImage> vk_swapchain_images;
                std::vector<s_swapchain_image_info> vk_swapchain_image_buffer;

                bool init_procs()
                {
                    GET_INSTANCE_PROC_ADDR(vk_instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
                    GET_INSTANCE_PROC_ADDR(vk_instance, GetPhysicalDeviceSurfacePresentModesKHR);
                    GET_DEVICE_PROC_ADDR(vk_device, CreateSwapchainKHR);
                    GET_DEVICE_PROC_ADDR(vk_device, DestroySwapchainKHR);
                    GET_DEVICE_PROC_ADDR(vk_device, GetSwapchainImagesKHR);
                    GET_DEVICE_PROC_ADDR(vk_device, AcquireNextImageKHR);
                    GET_DEVICE_PROC_ADDR(vk_device, QueuePresentKHR);
                    return true;
                }

                bool init(core::c_window* window)
                {
                    if (!init_procs())
                    {
                        return false;
                    }

                    VkSwapchainKHR old_swapchain = vk_swapchain;

                    // Get physical device surface properties and formats
                    VkSurfaceCapabilitiesKHR surface_caps;
                    VK_VERIFY(GetPhysicalDeviceSurfaceCapabilitiesKHR(vk_gpu, vk_surface, &surface_caps));

                    uint32_t present_mode_count;
                    VK_VERIFY(GetPhysicalDeviceSurfacePresentModesKHR(vk_gpu, vk_surface, &present_mode_count, nullptr));

                    // todo : replace with vector?
                    std::vector<VkPresentModeKHR> present_modes(present_mode_count);

                    VK_VERIFY(GetPhysicalDeviceSurfacePresentModesKHR(vk_gpu, vk_surface, &present_mode_count,
                                                                      present_modes.data()));

                    VkExtent2D swapchain_extent = {};
                    // width and height are either both -1, or both not -1.
                    if (surface_caps.currentExtent.width == -1)
                    {
                        glm::ivec2 size = window->get_size();
                        swapchain_extent.width = size.x;
                        swapchain_extent.height = size.y;
                    }
                    else
                    {
                        swapchain_extent = surface_caps.currentExtent;
                    }

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
                    swapchain_create_info.surface = vk_surface;
                    swapchain_create_info.minImageCount = desired_mumber_of_swapchain_images;
                    swapchain_create_info.imageFormat = vk_surface_format.format;
                    swapchain_create_info.imageColorSpace = vk_surface_format.colorSpace;
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

                    VK_VERIFY(CreateSwapchainKHR(vk_device, &swapchain_create_info, nullptr, &vk_swapchain));

                    if (old_swapchain != VK_NULL_HANDLE)
                    {
                        DestroySwapchainKHR(vk_device, old_swapchain, nullptr);
                    }

                    VK_VERIFY(GetSwapchainImagesKHR(vk_device, vk_swapchain, &vk_image_count, nullptr));

                    vk_swapchain_images.resize(vk_image_count);

                    VK_VERIFY(GetSwapchainImagesKHR(vk_device, vk_swapchain, &vk_image_count, vk_swapchain_images.data()));

                    vk_swapchain_image_buffer.resize(vk_image_count);
                    for (uint32_t i = 0; i < vk_image_count; i++)
                    {
                        auto& buffer = vk_swapchain_image_buffer[i];
                        buffer.image = vk_swapchain_images[i];

                        VkImageViewCreateInfo image_view_create_info = {};
                        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                        image_view_create_info.pNext = nullptr;
                        image_view_create_info.format = vk_surface_format.format;
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

                        utils::set_image_layout(resources::vk_setup_command_buffer, buffer.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                        VK_VERIFY(vkCreateImageView(vk_device, &image_view_create_info, nullptr, &buffer.view));
                    }

                    return true;
                }

                void shutdown()
                {
                    for (uint32_t i = 0; i < vk_swapchain_images.size(); i++) {
                        vkDestroyImageView(vk_device, vk_swapchain_image_buffer[i].view, nullptr);
                    }
                    vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
                    vk_swapchain_image_buffer.clear();
                    vk_swapchain_images.clear();
                }
            }
        }
    }
}