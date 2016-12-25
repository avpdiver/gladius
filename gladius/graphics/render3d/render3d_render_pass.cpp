//
// Created by alexa on 020 20 03 2016.
//

#include "render3d_globals.h"
#include "render3d_render_pass.h"
#include "render3d_macros.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

void get_default_attachment(VkFormat format, VkSampleCountFlagBits samples,
                            VkAttachmentDescription &attachment) {
    attachment.format = format;
    attachment.samples = samples;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.flags = 0;
}

VkRenderPass create_render_pass(const VkAttachmentDescription *attachments, size_t attachments_count,
                                const VkSubpassDescription *subpasses, size_t subpasses_count) {
    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.attachmentCount = attachments_count;
    create_info.pAttachments = attachments;
    create_info.subpassCount = subpasses_count;
    create_info.pSubpasses = subpasses;
    create_info.dependencyCount = 0;
    create_info.pDependencies = nullptr;

    VkRenderPass render_pass;

    VK_VERIFY_RETURN(vkCreateRenderPass(vk_globals::device, &create_info, nullptr, &render_pass),
                     nullptr);

    return render_pass;
}

}
}
}
}