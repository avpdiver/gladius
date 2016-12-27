//
// Created by alexa on 027 27 12 2016.
//

#include "subpass_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool s_subpass_json::create(s_subpass_desc& desc) const {

    desc.m_description = {
            flags,
            pipelineBindPoint,
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr,
            0,
            nullptr
    };

    if ((desc.m_description.inputAttachmentCount = inputAttachments.size()) > 0) {
        for (const auto &a : inputAttachments) {
            desc.m_input_attachment_references.push_back(std::move(a));
        }
        desc.m_description.pInputAttachments = &(desc.m_input_attachment_references[0]);
    }

    if ((desc.m_description.colorAttachmentCount = colorAttachments.size()) > 0) {
        for (const auto &a : colorAttachments) {
            desc.m_color_attachment_references.push_back(std::move(a));
        }
        desc.m_description.pColorAttachments = &(desc.m_color_attachment_references[0]);
    }

    if (resolveAttachments.size() > 0) {
        for (const auto &a : resolveAttachments) {
            desc.m_resolve_attachment_references.push_back(std::move(a));
        }
        desc.m_description.pResolveAttachments = &(desc.m_resolve_attachment_references[0]);
    }

    if (depthStencilAttachment.size() > 0) {
        for (const auto &a : depthStencilAttachment) {
            desc.m_depthstencil_attachment_references.push_back(std::move(a));
        }
        desc.m_description.pDepthStencilAttachment = &(desc.m_depthstencil_attachment_references[0]);
    }

    if ((desc.m_description.preserveAttachmentCount = preserveAttachments.size()) > 0) {
        for (const auto &a : preserveAttachments) {
            desc.m_preserve_attachment_references.push_back(a);
        }
        desc.m_description.pPreserveAttachments = &(desc.m_preserve_attachment_references[0]);
    }

    return true;
}

}
}
}