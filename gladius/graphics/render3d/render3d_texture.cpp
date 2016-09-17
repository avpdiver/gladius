//
// Created by alexa on 023 23 02 2016.
//

#include <gli/gli.hpp>

#include "../../core/memory/allocator.h"
#include "../../core/memory/default_policies.h"
#include "../../core/memory/alloc_policies/lockfree_pool.h"

#include "render3d_globals.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

constexpr size_t TEXTURES_NUMBER = 32;

struct s_texture {
    uint32_t width, height, depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    VkFormat format;
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
    VkImageLayout image_layout;
};

typedef typename std::aligned_storage<sizeof(s_texture), alignof(s_texture)>::type s_texture_t;

core::memory::c_allocator<
        std::array<s_texture_t, TEXTURES_NUMBER>,
        core::memory::c_lockfree_pool<s_texture_t>,
        core::memory::c_no_thread_policy,
        core::memory::c_no_bounds_policy,
        core::memory::c_no_tracking_policy,
        core::memory::c_no_tagging_policy> g_texture_pool;


bool create_image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t mip_levels, uint32_t array_layers, VkImage *image) {
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
            VK_SAMPLE_COUNT_1_BIT,                                // VkSampleCountFlagBits      samples
            VK_IMAGE_TILING_OPTIMAL,                              // VkImageTiling              tiling
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |                     // VkImageUsageFlags          usage
            VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SHARING_MODE_EXCLUSIVE,                            // VkSharingMode              sharingMode
            0,                                                    // uint32_t                   queueFamilyIndexCount
            nullptr,                                              // const uint32_t*            pQueueFamilyIndices
            VK_IMAGE_LAYOUT_UNDEFINED                             // VkImageLayout              initialLayout
    };

    VK_VERIFY(vkCreateImage(vk_globals::device, &image_create_info, nullptr, image));
    return true;
}

bool create_image_view(VkImage image, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkImageView* view) {
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

bool allocate_memory(VkImage image, VkMemoryPropertyFlagBits property, VkDeviceMemory *memory) {
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(vk_globals::device, image, &requirements);

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(vk_globals::gpu, &properties);

    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) && (properties.memoryTypes[i].propertyFlags & property)) {

            VkMemoryAllocateInfo memory_allocate_info = {
                    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
                    nullptr,                                    // const void                            *pNext
                    requirements.size,                          // VkDeviceSize                           allocationSize
                    i                                           // uint32_t                               memoryTypeIndex
            };

            VK_VERIFY(vkAllocateMemory(vk_globals::device, &memory_allocate_info, nullptr, memory));
            return true;
        }
    }
    VERIFY_LOG(false, LOG_TYPE, "Failed allocate image memory", "");
}

bool load_texture(const char *filename, handle_t& handle) {
    gli::texture2d tex2d(gli::load(filename));
    VERIFY_LOG(!tex2d.empty(), LOG_TYPE, "Failed to load texture %s", filename);

    s_texture tex;
    tex.width = tex2d[0].extent().x;
    tex.height = tex2d[0].extent().y;
    tex.mip_levels = tex2d.levels();
    tex.format = (VkFormat) tex2d.format();

    VERIFY(create_image(tex.format, tex.width, tex.height, 1, tex.mip_levels, 1, &tex.image));
    VERIFY(allocate_memory(tex.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &tex.memory));
    VK_VERIFY(vkBindImageMemory(vk_globals::device, tex.image, tex.memory, 0));
    VERIFY(create_image_view(tex.image, tex.format, tex.mip_levels, tex.array_layers, &tex.view));

    s_texture* texture = (s_texture*)g_texture_pool.alloc(1);
    (*texture) = std::move(tex);
    handle = reinterpret_cast<handle_t>(texture);

    return true;
}

}
}
}
}