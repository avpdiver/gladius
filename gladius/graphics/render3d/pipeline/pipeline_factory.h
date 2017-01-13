//
// Created by alexa on 029 29 12 2016.
//

#ifndef GLADIUS_PIPELINE_H
#define GLADIUS_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../../../core/types.h"
#include "../resources/texture.h"
#include "../renderer3d.h"

namespace gladius {
namespace graphics {
namespace render3d {

class c_pipeline {
public:
    c_pipeline(c_renderer3d* renderer);
    ~c_pipeline();

public:
    NOT_COPYABLE(c_pipeline);
    NOT_MOVEABLE(c_pipeline);

public:
    bool create();

private:
    bool create_depth_stencil();
    bool create_render_pass();
    bool create_descriptor_set();
    bool create_pipeline();
    bool create_framebuffer();

private:
    c_renderer3d* m_renderer;

private:
    resources::texture_handle m_depth_stencil = INVALID_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_framebuffers;
};

}
}
}

#endif //GLADIUS_PIPELINE_H
