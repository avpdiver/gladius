//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_RENDERPASS_DESC_H
#define GLADIUS_RENDERPASS_DESC_H


#include <vulkan/vulkan.h>
#include "../../../core/meta/json.h"
#include "../render3d_utils.h"
#include "subpass_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_renderpass_attachment_json : public VkAttachmentDescription {
public:
    JSON_FIELDS(
            JSON_FIELD(s_renderpass_attachment_json, flags),
            JSON_FIELD_CONV(s_renderpass_attachment_json, format, utils::string_to_format),
            JSON_FIELD_CONV(s_renderpass_attachment_json, samples, utils::get_sample_count),
            JSON_FIELD_CONV(s_renderpass_attachment_json, loadOp, utils::string_to_load_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, storeOp, utils::string_to_store_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, stencilLoadOp, utils::string_to_load_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, stencilStoreOp, utils::string_to_store_op),
            JSON_FIELD_CONV(s_renderpass_attachment_json, initialLayout, utils::string_to_image_layout),
            JSON_FIELD_CONV(s_renderpass_attachment_json, finalLayout, utils::string_to_image_layout)
    );

public:
    VkAttachmentDescription&& create() const;
};

struct s_renderpass_json {
public:
    std::vector<s_renderpass_attachment_json> attachments;
    std::vector<s_subpass_json> subpasses;

    JSON_FIELDS(
            JSON_FIELD(s_renderpass_json, attachments),
            JSON_FIELD(s_renderpass_json, subpasses)
    );

public:
    VkRenderPass create() const;
};

}
}
}

#endif //GLADIUS_RENDERPASS_DESC_H
