//
// Created by alexa on 022 22 02 2016.
//

#include "renderer3d_common.h"

namespace gladius
{
    namespace graphics
    {
        namespace renderer3d
        {
            namespace utils
            {
                void set_image_layout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                                      VkImageLayout old_image_layout, VkImageLayout new_image_layout)
                {
                    VkImageMemoryBarrier image_memory_barrier = {};
                    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    image_memory_barrier.pNext = nullptr;
                    image_memory_barrier.srcAccessMask = 0;
                    image_memory_barrier.dstAccessMask = 0;
                    image_memory_barrier.oldLayout = old_image_layout;
                    image_memory_barrier.newLayout = new_image_layout;
                    image_memory_barrier.image = image;
                    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
                    image_memory_barrier.subresourceRange.baseMipLevel = 0;
                    image_memory_barrier.subresourceRange.levelCount = 1;
                    image_memory_barrier.subresourceRange.layerCount = 1;

                    if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    }

                    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                    {
                        /* Make sure anything that was copying from this image has completed */
                        image_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                    }

                    if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        /* Make sure any Copy or CPU writes to image are flushed */
                        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    }

                    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                    }

                    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                    }

                    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                    vkCmdPipelineBarrier(commandBuffer, src_stages, dest_stages, 0, 0, nullptr, 0, nullptr, 1,
                                         &image_memory_barrier);
                }
            }
        }
    }
}