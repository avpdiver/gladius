//
// Created by alexa on 027 27 12 2016.
//

#ifndef GLADIUS_VIEWPORT_STATE_DESC_H
#define GLADIUS_VIEWPORT_STATE_DESC_H

#include "pipeline_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {

struct s_viewport_state_desc {
public:
    VkPipelineViewportStateCreateInfo m_create_info;
    std::vector<VkViewport> m_viewports;
    std::vector<VkRect2D> m_scissors;

public:
    s_viewport_state_desc&& get() const;
};

struct s_viewport_json : public VkViewport {
public:
    JSON_FIELDS(
            JSON_FIELD(s_viewport_json, x),
            JSON_FIELD(s_viewport_json, y),
            JSON_FIELD_CONV(s_viewport_json, width, float_width_func),
            JSON_FIELD_CONV(s_viewport_json, height, float_height_func),
            JSON_FIELD(s_viewport_json, minDepth),
            JSON_FIELD(s_viewport_json, maxDepth)
    );

public:
    VkViewport&& create() const;
};

struct s_scissor_json {
public:
    int32_t xOffset = 0;
    int32_t yOffset = 0;
    float width = 1.0f;
    float height = 1.0f;

    JSON_FIELDS(
            JSON_FIELD(s_scissor_json, xOffset),
            JSON_FIELD(s_scissor_json, yOffset),
            JSON_FIELD_CONV(s_scissor_json, width, float_width_func),
            JSON_FIELD_CONV(s_scissor_json, height, float_height_func)
    );

public:
    VkRect2D&& create() const;
};

struct s_viewport_state_json {
public:
    std::vector<s_viewport_json> viewports;
    std::vector<s_scissor_json> scissors;

    JSON_FIELDS(
            JSON_FIELD(s_viewport_state_json, viewports),
            JSON_FIELD(s_viewport_state_json, scissors)
    );

public:
    bool create(s_viewport_state_desc& des) const;
};

}
}
}

#endif //GLADIUS_VIEWPORT_STATE_DESC_H
