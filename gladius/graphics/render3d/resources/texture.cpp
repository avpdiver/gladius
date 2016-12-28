//
// Created by alexa on 023 23 02 2016.
//

#define GLM_ENABLE_EXPERIMENTAL

#include <gli/gli.hpp>

#include "../../../core/types.h"
#include "../../../core/memory/alloc_policies/lockfree_alloc.h"
#include "../../../core/memory/allocator.h"

#include "../render3d_macros.h"
#include "../render3d_debug.h"
#include "../render3d_globals.h"
#include "../render3d_resources.h"

#include "texture.h"
#include "buffer.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {


struct s_texture_desc {
public:
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    VkFormat format;
    VkImageLayout image_layout;
    VkSampleCountFlagBits samples;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    memory::c_block memory_block;

public:
    s_texture_desc() = default;
    ~s_texture_desc();

public:
    MOVEABLE(s_texture_desc);
    NOT_COPYABLE(s_texture_desc);
};

DEFAULT_MOVE_IMPL(s_texture_desc)

constexpr size_t RESOURCES_NUMBER = 32;

RESOURCE_POOL(s_texture_desc, RESOURCES_NUMBER);

bool create_image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples,
                  VkImageUsageFlags usage, VkImage *image) {
    VkImageCreateInfo image_create_info = {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                  // VkStructureType            sType;
            nullptr,                                              // const void                *pNext
            0,                                                    // VkImageCreateFlags         flags
            VK_IMAGE_TYPE_2D,                                     // VkImageType                imageType
            format,                                               // VkFormat                   format
            {                                                     // VkExtent3D                 extent
                    width,                                        // uint32_t                   width
                    height,                                       // uint32_t                   height
                    depth                                         // uint32_t                   depth
            },
            mip_levels,                                           // uint32_t                   mipLevels
            array_layers,                                         // uint32_t                   arrayLayers
            samples,                                              // VkSampleCountFlagBits      samples
            VK_IMAGE_TILING_OPTIMAL,                              // VkImageTiling              tiling
            usage,                                                // VkImageUsageFlags          usage
            VK_SHARING_MODE_EXCLUSIVE,                            // VkSharingMode              sharingMode
            0,                                                    // uint32_t                   queueFamilyIndexCount
            nullptr,                                              // const uint32_t*            pQueueFamilyIndices
            VK_IMAGE_LAYOUT_UNDEFINED                             // VkImageLayout              initialLayout
    };

    VK_VERIFY(vkCreateImage(vk_globals::device, &image_create_info, nullptr, image));
    return true;
}

bool create_image_view(VkImage image, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkImageView *view) {
    VkImageViewCreateInfo image_view_create_info =
            {
                    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
                    nullptr,                                    // const void                    *pNext
                    0,                                          // VkImageViewCreateFlags         flags
                    image,                                      // VkImage                        image
                    VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
                    format,                                     // VkFormat                       format
                    {                                           // VkComponentMapping             components
                            VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             r
                            VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             g
                            VK_COMPONENT_SWIZZLE_IDENTITY,      // VkComponentSwizzle             b
                            VK_COMPONENT_SWIZZLE_IDENTITY       // VkComponentSwizzle             a
                    },
                    {                                           // VkImageSubresourceRange        subresourceRange
                            VK_IMAGE_ASPECT_COLOR_BIT,          // VkImageAspectFlags             aspectMask
                            0,                                  // uint32_t                       baseMipLevel
                            mip_levels,                         // uint32_t                       levelCount
                            0,                                  // uint32_t                       baseArrayLayer
                            array_layers                        // uint32_t                       layerCount
                    }
            };

    VK_VERIFY (vkCreateImageView(vk_globals::device, &image_view_create_info, nullptr, view));
    return true;
}

bool create_sampler(VkSampler *sampler) {
    VkSamplerCreateInfo sampler_create_info = {
            VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // VkStructureType            sType
            nullptr,                                              // const void*                pNext
            0,                                                    // VkSamplerCreateFlags       flags
            VK_FILTER_LINEAR,                                     // VkFilter                   magFilter
            VK_FILTER_LINEAR,                                     // VkFilter                   minFilter
            VK_SAMPLER_MIPMAP_MODE_NEAREST,                       // VkSamplerMipmapMode        mipmapMode
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeU
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeV
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,                // VkSamplerAddressMode       addressModeW
            0.0f,                                                 // float                      mipLodBias
            VK_FALSE,                                             // VkBool32                   anisotropyEnable
            1.0f,                                                 // float                      maxAnisotropy
            VK_FALSE,                                             // VkBool32                   compareEnable
            VK_COMPARE_OP_ALWAYS,                                 // VkCompareOp                compareOp
            0.0f,                                                 // float                      minLod
            0.0f,                                                 // float                      maxLod
            VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,              // VkBorderColor              borderColor
            VK_FALSE                                              // VkBool32                   unnormalizedCoordinates
    };

    VK_VERIFY (vkCreateSampler(vk_globals::device, &sampler_create_info, nullptr, sampler));
    return true;
}

bool copy_texture_data(const gli::texture2d &tex2d, s_texture_desc &desc) {
    /*
    // Prepare data in staging buffer
    resources::s_render_context const *render_context;
    VERIFY(resources::get_current_render_context(&render_context));

    auto staging_buffer = reinterpret_cast<s_buffer_desc*>(render_context->staging_buffer);

    // Copy texture data into staging buffer
    uint8_t *data;
    VK_VERIFY(vkMapMemory(vk_globals::device, staging_buffer->memory, 0, tex2d.size(), 0,
                          (void **) &data));
    memcpy(data, tex2d.data(), tex2d.size());
    VkMappedMemoryRange flush_range = {
            VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // VkStructureType                        sType
            nullptr,                                            // const void                            *pNext
            staging_buffer->memory,                           // VkDeviceMemory                         memory
            0,                                                  // VkDeviceSize                           offset
            tex2d.size()                                        // VkDeviceSize                           size
    };
    vkFlushMappedMemoryRanges(vk_globals::device, 1, &flush_range);
    vkUnmapMemory(vk_globals::device, staging_buffer->memory);


    // Prepare command buffer to copy data from staging buffer to a image
    VkCommandBuffer command_buffer = render_context->command_buffer;
    VkCommandBufferBeginInfo begin_info = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // VkStructureType                        sType
            nullptr,                                            // const void                            *pNext
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,        // VkCommandBufferUsageFlags              flags
            nullptr                                             // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
    };

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkImageSubresourceRange image_subresource_range = {
            VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
            0,                                                  // uint32_t                               baseMipLevel
            desc.mip_levels,                                    // uint32_t                               levelCount
            0,                                                  // uint32_t                               baseArrayLayer
            desc.array_layers                                   // uint32_t                               layerCount
    };

    VkImageMemoryBarrier from_undefined_to_transfer_dst = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // VkStructureType                        sType
            nullptr,                                            // const void                            *pNext
            0,                                                  // VkAccessFlags                          srcAccessMask
            VK_ACCESS_TRANSFER_WRITE_BIT,                       // VkAccessFlags                          dstAccessMask
            VK_IMAGE_LAYOUT_UNDEFINED,                          // VkImageLayout                          oldLayout
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,               // VkImageLayout                          newLayout
            VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               dstQueueFamilyIndex
            desc.image,                                         // VkImage                                image
            image_subresource_range                             // VkImageSubresourceRange                subresourceRange
    };
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &from_undefined_to_transfer_dst);


    // Setup buffer copy regions for each mip level
    std::vector<VkBufferImageCopy> buffer_copy_regions;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < desc.mip_levels; i++) {
        VkBufferImageCopy buffer_image_copy_info = {
                offset,                                             // VkDeviceSize                           bufferOffset
                0,                                                  // uint32_t                               bufferRowLength
                0,                                                  // uint32_t                               bufferImageHeight
                {                                                   // VkImageSubresourceLayers               imageSubresource
                        VK_IMAGE_ASPECT_COLOR_BIT,                  // VkImageAspectFlags                     aspectMask
                        i,                                          // uint32_t                               mipLevel
                        0,                                          // uint32_t                               baseArrayLayer
                        desc.array_layers                           // uint32_t                               layerCount
                },
                {                                                   // VkOffset3D                             imageOffset
                        0,                                          // int32_t                                x
                        0,                                          // int32_t                                y
                        0                                           // int32_t                                z
                },
                {                                                   // VkExtent3D                             imageExtent
                        static_cast<uint32_t>(tex2d[i].extent().x), // uint32_t                               width
                        static_cast<uint32_t>(tex2d[i].extent().y), // uint32_t                               height
                        1                                           // uint32_t                               depth
                }
        };
        buffer_copy_regions.push_back(buffer_image_copy_info);
        offset += static_cast<uint32_t>(tex2d[i].size());
    }
    vkCmdCopyBufferToImage(command_buffer, staging_buffer->handle, desc.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, buffer_copy_regions.size(),
                           buffer_copy_regions.data());

    VkImageMemoryBarrier from_transfer_to_shader_read = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // VkStructureType                        sType
            nullptr,                                            // const void                            *pNext
            VK_ACCESS_TRANSFER_WRITE_BIT,                       // VkAccessFlags                          srcAccessMask
            VK_ACCESS_SHADER_READ_BIT,                          // VkAccessFlags                          dstAccessMask
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,               // VkImageLayout                          oldLayout
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,           // VkImageLayout                          newLayout
            VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                            // uint32_t                               dstQueueFamilyIndex
            desc.image,                                         // VkImage                                image
            image_subresource_range                             // VkImageSubresourceRange                subresourceRange
    };
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &from_transfer_to_shader_read);

    vkEndCommandBuffer(command_buffer);

    // Submit command buffer and copy data from staging buffer to a vertex buffer
    VkSubmitInfo submit_info = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,                      // VkStructureType                        sType
            nullptr,                                            // const void                            *pNext
            0,                                                  // uint32_t                               waitSemaphoreCount
            nullptr,                                            // const VkSemaphore                     *pWaitSemaphores
            nullptr,                                            // const VkPipelineStageFlags            *pWaitDstStageMask;
            1,                                                  // uint32_t                               commandBufferCount
            &command_buffer,                                    // const VkCommandBuffer                 *pCommandBuffers
            0,                                                  // uint32_t                               signalSemaphoreCount
            nullptr                                             // const VkSemaphore                     *pSignalSemaphores
    };

    VK_VERIFY (vkQueueSubmit(vk_globals::graphics_queue.handle, 1, &submit_info, VK_NULL_HANDLE));
    vkDeviceWaitIdle(vk_globals::device);
*/
    return true;
}

bool load_texture(const char *filename, texture_handle *handle) {
    gli::texture2d tex2d(gli::load(filename));
    VERIFY_LOG(!tex2d.empty(), LOG_TYPE, "Failed to load texture %s", filename);

    s_texture_desc tex;
    tex.width = tex2d[0].extent().x;
    tex.height = tex2d[0].extent().y;
    tex.mip_levels = tex2d.levels();
    tex.format = (VkFormat) tex2d.format();

    VERIFY(create_image(tex.format, tex.width, tex.height, 1, tex.mip_levels, 1, VK_SAMPLE_COUNT_1_BIT,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, &tex.image));

    tex.memory_block = vk_globals::gpu_memory_allocator->alloc(tex.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_VERIFY(vkBindImageMemory(vk_globals::device, tex.image, tex.memory_block.m_memory, tex.memory_block.m_offset));
    VERIFY(create_image_view(tex.image, tex.format, tex.mip_levels, tex.array_layers, &tex.view));
    VERIFY(copy_texture_data(tex2d, tex));

    s_texture_desc *texture = (s_texture_desc *) g_resource_pool.alloc(1);
    (*texture) = std::move(tex);
    (*handle) = reinterpret_cast<texture_handle>(texture);

    return true;
}

bool create_texture(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                    uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
                    texture_handle *handle) {

    VkImage image;
    VkImageView view;

    VERIFY(create_image(format, width, height, 1, mip_levels, array_layers, samples, usage, &image));

    auto memory_block = vk_globals::gpu_memory_allocator->alloc(image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_VERIFY(vkBindImageMemory(vk_globals::device, image, memory_block.m_memory, memory_block.m_offset));
    VERIFY(create_image_view(image, format, mip_levels, array_layers, &view));

    s_texture_desc *resource = (s_texture_desc *) g_resource_pool.alloc(1);
    resource->width = width;
    resource->height = height;
    resource->depth = depth;
    resource->mip_levels = mip_levels;
    resource->array_layers = array_layers;
    resource->format = format;
    resource->samples = samples;
    resource->image = image;
    resource->view = view;
    resource->memory_block = std::move(memory_block);

    (*handle) = reinterpret_cast<texture_handle>(resource);

    return true;
}

VkImageView get_texture_image_view(texture_handle handle) {
    if (handle == INVALID_HANDLE) {
        return VK_NULL_HANDLE;
    }
    return reinterpret_cast<s_texture_desc*>(handle)->view;
}

void destroy(s_texture_desc* desc) {
    vk_globals::gpu_memory_allocator->free(desc->memory_block);
    vkDestroyImageView(vk_globals::device, desc->view, nullptr);
    vkDestroyImage(vk_globals::device, desc->image, nullptr);
}

void destroy_texture(texture_handle handle) {
    if (handle == INVALID_HANDLE) {
        return;
    }
    s_texture_desc* desc = reinterpret_cast<s_texture_desc*>(handle);
    destroy(desc);
    g_resource_pool.free(desc);
}

s_texture_desc::~s_texture_desc() {
    destroy(this);
}

}
}
}
}