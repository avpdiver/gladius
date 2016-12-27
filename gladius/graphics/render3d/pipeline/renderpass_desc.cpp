//
// Created by alexa on 027 27 12 2016.
//

#include "renderpass_desc.h"
#include "../../../core/logger/logger.h"
#include "../render3d_globals.h"
#include "../render3d_macros.h"


namespace gladius {
namespace graphics {
namespace render3d {

VkAttachmentDescription&& s_renderpass_attachment_json::create() const {
    return std::move(*(VkAttachmentDescription*)this);
}

VkRenderPass s_renderpass_json::create() const {
    VkRenderPassCreateInfo create_info  = {
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            nullptr,
            0
    };

    if ((create_info.attachmentCount = attachments.size()) > 0) {
        create_info.pAttachments = &(attachments[0]);
    }

    std::vector<VkSubpassDescription> subpass_descriptions;
    std::vector<s_subpass_desc> subpass_descs;
    if ((create_info.subpassCount = subpasses.size()) > 0) {
        for (const auto &a : subpasses) {
            s_subpass_desc subpass_desc;
            VERIFY_RETURN(a.create(subpass_desc), VK_NULL_HANDLE);
            subpass_descriptions.push_back(subpass_desc.m_description);
            subpass_descs.push_back(std::move(subpass_desc));
        }
        create_info.pSubpasses = &(subpass_descriptions[0]);
    }

    create_info.dependencyCount = 0;
    create_info.pDependencies = nullptr;

    VkRenderPass renderPass;
    VK_VERIFY_RETURN(vkCreateRenderPass(vk_globals::device, &create_info, nullptr, &renderPass), VK_NULL_HANDLE);

    return renderPass;
}

}
}
}