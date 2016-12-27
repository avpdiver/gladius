//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_SUBPASS_DESC_H
#define GLADIUS_SUBPASS_DESC_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../../../core/meta/json.h"
#include "../render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_subpass_desc {
    VkSubpassDescription m_description;
    std::vector <VkAttachmentReference> m_input_attachment_references;
    std::vector <VkAttachmentReference> m_color_attachment_references;
    std::vector <VkAttachmentReference> m_resolve_attachment_references;
    std::vector <VkAttachmentReference> m_depthstencil_attachment_references;
    std::vector <uint32_t> m_preserve_attachment_references;
};

struct s_attachment_ref_json : public VkAttachmentReference {
    JSON_FIELDS(
            JSON_FIELD(s_attachment_ref_json, attachment),
            JSON_FIELD_CONV(s_attachment_ref_json, layout, utils::string_to_image_layout)
    );
};

struct s_subpass_json {
public:
    VkSubpassDescriptionFlags flags;
    VkPipelineBindPoint pipelineBindPoint;
    std::vector<s_attachment_ref_json> inputAttachments;
    std::vector<s_attachment_ref_json> colorAttachments;
    std::vector<s_attachment_ref_json> resolveAttachments;
    std::vector<s_attachment_ref_json> depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;

    JSON_FIELDS(
            JSON_FIELD(s_subpass_json, flags),
            JSON_FIELD_CONV(s_subpass_json, pipelineBindPoint, utils::string_to_bind_point),
            JSON_FIELD(s_subpass_json, inputAttachments),
            JSON_FIELD(s_subpass_json, colorAttachments),
            JSON_FIELD(s_subpass_json, resolveAttachments),
            JSON_FIELD(s_subpass_json, depthStencilAttachment),
            JSON_FIELD(s_subpass_json, preserveAttachments)
    );

public:
    bool create(s_subpass_desc& desc) const;
};

}
}
}

#endif //GLADIUS_SUBPASS_DESC_H
