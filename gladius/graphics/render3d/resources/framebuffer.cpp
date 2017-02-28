//
// Created by alexa on 027 27 12 2016.
//


#include "../../../core/memory/alloc_policies/lockfree_alloc.h"
#include "../../../core/memory/allocator.h"

#include "../renderer3d.h"
#include "../vulkan/vulkan_macros.h"

#include "texture.h"
#include "framebuffer.h"
#include "resource_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_framebuffer_desc {
public:

public:
    s_framebuffer_attachment attachments[8];
    size_t attachmentCount = 0;
    VkFramebuffer handle = VK_NULL_HANDLE;

public:
    s_framebuffer_desc() = default;
    ~s_framebuffer_desc();

public:
    MOVEABLE(s_framebuffer_desc);
    NOT_COPYABLE(s_framebuffer_desc);
};

constexpr size_t RESOURCES_NUMBER = 32;

RESOURCE_POOL(s_framebuffer_desc, RESOURCES_NUMBER);

bool create_framebuffer(uint32_t width, uint32_t height,
                        const std::vector <s_framebuffer_attachment> &attachments, VkRenderPass render_pass,
                        framebuffer_handle *handle) {
    std::vector<VkImageView> images;

    for (const auto &attachment : attachments) {
        if (attachment.texture != INVALID_HANDLE) {
            images.push_back(get_texture_image_view(attachment.texture));
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
    VK_VERIFY(vkCreateFramebuffer(renderer3d.m_device, &create_info, nullptr, &framebuffer));

    auto resource = (s_framebuffer_desc *) g_resource_pool.alloc(1);
    resource->handle = framebuffer;
    resource->attachmentCount = attachments.size();
    memcpy(resource->attachments, &(attachments[0]), attachments.size() * sizeof(s_framebuffer_attachment));
    *handle = reinterpret_cast<framebuffer_handle>(resource);

    return true;
}

void destroy(s_framebuffer_desc* desc) {
    vkDestroyFramebuffer(renderer3d.m_device, desc->handle, nullptr);
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