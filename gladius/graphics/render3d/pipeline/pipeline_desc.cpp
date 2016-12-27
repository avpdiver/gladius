//
// Created by alexa on 027 27 12 2016.
//

#include "pipeline_desc.h"

namespace gladius {
namespace graphics {
namespace render3d {

bool s_pipelines_json::create(s_pipelines_desc& desc) const {

    for (const auto& v : viewport_states) {
        s_viewport_state_desc viewport_state_desc;
        VERIFY(v.create(viewport_state_desc));
        desc.m_viewport_states.push_back(std::move(viewport_state_desc));
    }

    for(const auto& r : rasterization_states) {
        desc.m_rasterization_states.push_back(r.create());
    }

    for (const auto& m : multisample_states) {
        desc.m_multisample_states.push_back(m.create());
    };

    for (const auto& c : color_blend_states) {
        s_color_blend_state_desc color_blend_state_desc;
        VERIFY(c.create(color_blend_state_desc));
        desc.m_color_blend_states.push_back(color_blend_state_desc);
    }

    for (const auto& rp : renderpasses) {
        auto r = rp.create();
        VERIFY(r != VK_NULL_HANDLE);
        desc.m_render_passes.push_back(r);
    }

    auto swapchain_desc = swapchain.create();
    create_swap_chain(swapchain_desc);

    for (const auto& f : framebuffers) {
        resources::framebuffer_handle handle;
        VERIFY(f.create(desc.m_render_passes, &handle));
        desc.m_framebuffers.push_back(handle);
    }

    return true;
}

bool s_pipelines_desc::create() const {
    return true;
}

}
}
}