//
// Created by alexa on 027 27 12 2016.
//

#include "render3d_framebuffer.h"
#include "../../core/logger/logger.h"
#include "render3d_globals.h"
#include "render3d_macros.h"
#include "../../core/memory/alloc_policies/lockfree_alloc.h"
#include "../../core/memory/allocator.h"
#include "render3d_texture.h"
#include "resources/framebuffer_resource.h"
#include "resources/texture_resource.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

constexpr size_t RESOURCES_NUMBER = 32;

typedef typename std::aligned_storage<sizeof(s_framebuffer_desc), alignof(s_framebuffer_desc)>::type s_framebuffer_t;
static core::memory::c_allocator<
        std::array<s_framebuffer_t, RESOURCES_NUMBER>,
        core::memory::c_lockfree_alloc<sizeof(s_framebuffer_t), alignof(s_framebuffer_t)>> g_resource_pool;

bool create_framebuffer(uint32_t width, uint32_t height,
                        const std::vector <s_framebuffer_desc::s_attachment> &attachments, VkRenderPass render_pass,
                        framebuffer_handle *handle) {
    std::vector<VkImageView> images;

    for (const auto &attachment : attachments) {
        if (attachment.texture != INVALID_HANDLE) {
            images.push_back(reinterpret_cast<s_texture_desc*>(attachment.texture)->view);
        } else if (attachment.image_view != VK_NULL_HANDLE){
            images.push_back(attachment.image_view);
        } else {
            VERIFY_LOG(false, LOG_TYPE, "Invalid framebuffer attachments", "");
        }
    }

    VkFramebufferCreateInfo create_info = {
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            nullptr,
            0,
            render_pass,
            (uint32_t) images.size(),
            &(images[0]),
            width,
            height,
            1
    };

    VkFramebuffer framebuffer;
    VK_VERIFY(vkCreateFramebuffer(vk_globals::device, &create_info, nullptr, &framebuffer));

    auto resource = (s_framebuffer_desc *) g_resource_pool.alloc(1);
    resource->handle = framebuffer;
    resource->attachmentCount = attachments.size();
    memcpy(resource->attachments, &(attachments[0]), attachments.size() * sizeof(s_framebuffer_desc::s_attachment));
    *handle = reinterpret_cast<framebuffer_handle>(resource);

    return true;
}

void destroy(s_framebuffer_desc* desc) {
    vkDestroyFramebuffer(vk_globals::device, desc->handle, nullptr);
    for (size_t i = 0; i < desc->attachmentCount; i++) {
        auto& attachment = desc->attachments[i];
        if (attachment.texture != INVALID_HANDLE && attachment.is_texture_owner) {
            destroy_texture(attachment.texture);
        }
    }
}

void destroy_framebuffer(const framebuffer_handle handle) {
    if (handle == INVALID_HANDLE) {
        return;
    }
    auto desc = reinterpret_cast<s_framebuffer_desc*>(handle);
    destroy(desc);
    g_resource_pool.free(desc);
}

s_framebuffer_desc::~s_framebuffer_desc() {
    destroy(this);
}

}
}
}
}