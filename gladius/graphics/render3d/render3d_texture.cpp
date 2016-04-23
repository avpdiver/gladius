//
// Created by alexa on 023 23 02 2016.
//

#include <gli/gli.hpp>

#include "render3d_globals.h"
#include "render3d_utils.h"
#include "render3d_command_buffer.h"

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace resources
            {
                struct s_texture
                {
                    VkSampler sampler;
                    VkImage image;
                    VkImageLayout image_layout;
                    VkDeviceMemory device_memory;
                    VkImageView view;
                    uint32_t width, height;
                    uint32_t mip_levels;
                };

                bool create_staging_texture(VkImageCreateInfo& image_create_info, s_texture& texture, gli::texture2d& tex2d)
                {
                    struct _s_mip_level
                    {
                        VkImage image;
                        VkDeviceMemory memory;
                    };

                    VkMemoryAllocateInfo mem_alloc_info = {};
                    VkMemoryRequirements mem_reqs;

                    std::vector<_s_mip_level> mip_levels;
                    mip_levels.resize(texture.mip_levels);

                    // Load mip levels into linear textures that are used to copy from
                    for (uint32_t level = 0; level < texture.mip_levels; level++)
                    {
                        image_create_info.extent.width = tex2d[level].extent().x;
                        image_create_info.extent.height = tex2d[level].extent().y;
                        image_create_info.extent.depth = 1;

                        VK_VERIFY(vkCreateImage(vk_globals::device, &image_create_info, nullptr, &mip_levels[level].image));

                        vkGetImageMemoryRequirements(vk_globals::device, mip_levels[level].image, &mem_reqs);
                        mem_alloc_info.allocationSize = mem_reqs.size;

                        mem_alloc_info.memoryTypeIndex = utils::get_memory_type(mem_reqs.memoryTypeBits,
                                                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                        VERIFY_LOG(mem_alloc_info.memoryTypeIndex >= 0, "There is no required memory type");

                        VK_VERIFY(vkAllocateMemory(vk_globals::device, &mem_alloc_info, nullptr, &mip_levels[level].memory));
                        VK_VERIFY(vkBindImageMemory(vk_globals::device, mip_levels[level].image, mip_levels[level].memory, 0));

                        VkImageSubresource subresource = {};
                        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                        VkSubresourceLayout subresource_layout;
                        void *data;

                        vkGetImageSubresourceLayout(vk_globals::device, mip_levels[level].image, &subresource, &subresource_layout);

                        VK_VERIFY(vkMapMemory(vk_globals::device, mip_levels[level].memory, 0, mem_reqs.size, 0, &data));
                        memcpy(data, tex2d[level].data(), tex2d[level].size());
                        vkUnmapMemory(vk_globals::device, mip_levels[level].memory);

                        // Image barrier for linear image (base)
                        // Linear image will be used as a source for the copy
                        utils::set_image_layout(vk_globals::present_command_buffers[0],
                                                mip_levels[level].image,
                                                VK_IMAGE_ASPECT_COLOR_BIT,
                                                VK_IMAGE_LAYOUT_PREINITIALIZED,
                                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
                    }

                    // Setup texture as blit target with optimal tiling
                    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
                    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    image_create_info.mipLevels = texture.mip_levels;
                    image_create_info.extent = { texture.width, texture.height, 1 };

                    VK_VERIFY(vkCreateImage(vk_globals::device, &image_create_info, nullptr, &texture.image));

                    vkGetImageMemoryRequirements(vk_globals::device, texture.image, &mem_reqs);

                    mem_alloc_info.allocationSize = mem_reqs.size;
                    mem_alloc_info.memoryTypeIndex = utils::get_memory_type(mem_reqs.memoryTypeBits,
                                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                    VERIFY_LOG(mem_alloc_info.memoryTypeIndex >= 0, "There is no required memory type");

                    VK_VERIFY(vkAllocateMemory(vk_globals::device, &mem_alloc_info, nullptr, &texture.device_memory));
                    VK_VERIFY(vkBindImageMemory(vk_globals::device, texture.image, texture.device_memory, 0));

                    // Image barrier for optimal image (target)
                    // Optimal image will be used as destination for the copy

                    // Set initial layout for all mip levels of the optimal (target) tiled texture
                    utils::set_image_layout(
                            vk_globals::present_command_buffers[0],
                            texture.image,
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            VK_IMAGE_LAYOUT_PREINITIALIZED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            0,
                            texture.mip_levels);

                    // Copy mip levels one by one
                    for (uint32_t level = 0; level < texture.mip_levels; ++level)
                    {
                        // Copy region for image blit
                        VkImageCopy copy_region = {};

                        copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        copy_region.srcSubresource.baseArrayLayer = 0;
                        copy_region.srcSubresource.mipLevel = 0;
                        copy_region.srcSubresource.layerCount = 1;
                        copy_region.srcOffset = {0, 0, 0};

                        copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        copy_region.dstSubresource.baseArrayLayer = 0;
                        copy_region.dstSubresource.mipLevel = level;
                        copy_region.dstSubresource.layerCount = 1;
                        copy_region.dstOffset = {0, 0, 0};

                        copy_region.extent.width = tex2d[level].extent().x;
                        copy_region.extent.height = tex2d[level].extent().y;
                        copy_region.extent.depth = 1;

                        // Put image copy into command buffer
                        vkCmdCopyImage(
                                vk_globals::present_command_buffers[0],
                                mip_levels[level].image,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                texture.image,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                1, &copy_region);
                    }

                    // Change texture image layout to shader read after all mip levels have been copied
                    texture.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    utils::set_image_layout(
                            vk_globals::present_command_buffers[0],
                            texture.image,
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            texture.image_layout,
                            0,
                            texture.mip_levels);

                    resources::flush_command_buffer(vk_globals::graphics_queue.handle, vk_globals::present_command_buffers[0]);

                    // Clean up linear images
                    // No longer required after mip levels
                    // have been transformed over to optimal tiling
                    for (auto& level : mip_levels)
                    {
                        vkDestroyImage(vk_globals::device, level.image, nullptr);
                        vkFreeMemory(vk_globals::device, level.memory, nullptr);
                    }

                    return true;
                }

                bool create_mapped_texture(VkImageCreateInfo& image_create_info, s_texture& texture, gli::texture2d& tex2d)
                {
                    VkMemoryAllocateInfo mem_alloc_info = {};
                    VkMemoryRequirements mem_reqs;

                    VkImage mappable_image;
                    VkDeviceMemory mappable_memory;

                    // Load mip map level 0 to linear tiling image
                    VK_VERIFY(vkCreateImage(vk_globals::device, &image_create_info, nullptr, &mappable_image));

                    vkGetImageMemoryRequirements(vk_globals::device, mappable_image, &mem_reqs);
                    mem_alloc_info.allocationSize = mem_reqs.size;
                    mem_alloc_info.memoryTypeIndex = utils::get_memory_type(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

                    // Allocate host memory
                    VK_VERIFY(vkAllocateMemory(vk_globals::device, &mem_alloc_info, nullptr, &mappable_memory));
                    VK_VERIFY(vkBindImageMemory(vk_globals::device, mappable_image, mappable_memory, 0));

                    // Get sub resource layout
                    // Mip map count, array layer, etc.
                    VkImageSubresource subresource = {};
                    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                    VkSubresourceLayout subresource_layout;
                    void *data;

                    // Get sub resources layout
                    // Includes row pitch, size offsets, etc.
                    vkGetImageSubresourceLayout(vk_globals::device, mappable_image, &subresource, &subresource_layout);

                    // Map image memory
                    VK_VERIFY(vkMapMemory(vk_globals::device, mappable_memory, 0, mem_reqs.size, 0, &data));
                    memcpy(data, tex2d[subresource.mipLevel].data(), tex2d[subresource.mipLevel].size());
                    vkUnmapMemory(vk_globals::device, mappable_memory);

                    // Linear tiled images don't need to be staged
                    // and can be directly used as textures
                    texture.image = mappable_image;
                    texture.device_memory = mappable_memory;
                    texture.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    // Setup image memory barrier
                    utils::set_image_layout(
                            vk_globals::present_command_buffers[0],
                            texture.image,
                            VK_IMAGE_ASPECT_COLOR_BIT,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            texture.image_layout,
                            0,
                            1);

                    return true;
                }

                bool load_texture(const char *filename, uint32_t image_format)
                {
                    VkFormatProperties formatProperties;

                    gli::texture2d tex2d(gli::load(filename));
                    VERIFY_LOGF(!tex2d.empty(), "Failed to load texture %s", filename);

                    VkFormat format = static_cast<VkFormat>(image_format);

                    s_texture texture = {};

                    texture.width = tex2d[0].extent().x;
                    texture.height = tex2d[0].extent().y;
                    texture.mip_levels = tex2d.levels();

                    vkGetPhysicalDeviceFormatProperties(vk_globals::gpu, format, &formatProperties);

                    VkBool32 use_staging = !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

                    VkImageCreateInfo image_create_info = {};
                    image_create_info.imageType = VK_IMAGE_TYPE_2D;
                    image_create_info.format = format;
                    image_create_info.mipLevels = 1;
                    image_create_info.arrayLayers = 1;
                    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
                    image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
                    image_create_info.usage = (use_staging) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT;
                    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                    image_create_info.flags = 0;
                    image_create_info.extent = { texture.width, texture.height, 1 };

                    if (use_staging)
                    {
                        VERIFY_LOG(create_staging_texture(image_create_info, texture, tex2d), "Failed create staged texture");
                    }
                    else
                    {
                        VERIFY_LOG(create_mapped_texture(image_create_info, texture, tex2d), "Failed create mapped texture");
                    }

                    // Create sampler
                    // In Vulkan textures are accessed by samplers
                    // This separates all the sampling information from the
                    // texture data
                    // This means you could have multiple sampler objects
                    // for the same texture with different settings
                    // Similar to the samplers available with OpenGL 3.3
                    VkSamplerCreateInfo sampler = {};
                    sampler.magFilter = VK_FILTER_LINEAR;
                    sampler.minFilter = VK_FILTER_LINEAR;
                    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                    sampler.mipLodBias = 0.0f;
                    sampler.compareOp = VK_COMPARE_OP_NEVER;
                    sampler.minLod = 0.0f;
                    sampler.maxLod = (use_staging) ? (float)texture.mip_levels : 0.0f;
                    sampler.maxAnisotropy = 8;
                    sampler.anisotropyEnable = VK_TRUE;
                    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                    VK_VERIFY(vkCreateSampler(vk_globals::device, &sampler, nullptr, &texture.sampler));

                    // Create image view
                    // Textures are not directly accessed by the shaders and
                    // are abstracted by image views containing additional
                    // information and sub resource ranges
                    VkImageViewCreateInfo view = {};
                    view.image = VK_NULL_HANDLE;
                    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    view.format = format;
                    view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
                    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    view.subresourceRange.baseMipLevel = 0;
                    view.subresourceRange.baseArrayLayer = 0;
                    view.subresourceRange.layerCount = 1;
                    view.subresourceRange.levelCount = (use_staging) ? texture.mip_levels : 1;
                    view.image = texture.image;
                    VK_VERIFY(vkCreateImageView(vk_globals::device, &view, nullptr, &texture.view));

                    return true;
                }
            }
        }
    }
}