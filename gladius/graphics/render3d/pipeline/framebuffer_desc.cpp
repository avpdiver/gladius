//
// Created by alexa on 027 27 12 2016.
//

#include "framebuffer_desc.h"
#include "../resources/framebuffer.h"

namespace gladius {
namespace graphics {
namespace render3d {

s_swapchain_desc s_swapchain_json::create() const {
    s_swapchain_desc desc;
    desc.format = format;
    desc.imageCount = imageCount;
    return desc;
}

bool s_framebuffer_json::create(std::vector<VkRenderPass>& renderpasses, resources::framebuffer_handle* handle) const {
    VERIFY_LOG_RETURN(!attachments.empty(), LOG_TYPE, VK_NULL_HANDLE, "There is no framebuffers attachments", "");

    std::vector<resources::s_framebuffer_attachment> textures;

    for (const auto &attachment : attachments) {
        if (attachment.swapchain < 0) {
            auto texture = attachment.create();
            textures.push_back({
                                       texture,
                                       VK_NULL_HANDLE,
                                       true
                               });
        } else {
            textures.push_back({
                                       INVALID_HANDLE,
                                       vk_globals::swapchain.views[attachment.swapchain],
                                       false
                               });
        }
    }

    VERIFY(resources::create_framebuffer(attachments[0].width, attachments[0].height, textures, renderpasses[renderpass], handle));
    return true;
}

}
}
}