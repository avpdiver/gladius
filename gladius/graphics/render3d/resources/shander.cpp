//
// Created by alexa on 004 04 09 2016.
//

#include <vector>

#include "../../../core/filesystem/filesystem.h"
#include "../../../core/memory/alloc_policies/lockfree_alloc.h"
#include "../../../core/memory/allocator.h"

#include "../renderer3d.h"
#include "../vulkan/vulkan_macros.h"

#include "shander.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

struct s_shader_desc {
public:
    VkShaderModule module;
    VkShaderStageFlagBits stage_flag;
    const char* entry;

public:
    s_shader_desc() = default;
    ~s_shader_desc();

public:
    NOT_MOVEABLE(s_shader_desc);
    NOT_COPYABLE(s_shader_desc);
};

constexpr size_t RESOURCES_NUMBER = 32;

RESOURCE_POOL(s_shader_desc, RESOURCES_NUMBER);

bool load_shader(const char *filename, e_shader_type shader_type, const char *entry, shander_handle* handle) {
    assert(filename != nullptr);
    assert(entry != nullptr);

    std::vector<char> code;
    VERIFY(core::filesystem::read("disk", filename, code));

    VkShaderModuleCreateInfo shader_module_create_info = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
            nullptr,                                        // const void                    *pNext
            0,                                              // VkShaderModuleCreateFlags      flags
            code.size(),                                    // size_t                         codeSize
            reinterpret_cast<const uint32_t *>(&code[0])     // const uint32_t                *pCode
    };

    VkShaderModule module;
    VK_VERIFY(vkCreateShaderModule(vk_globals::device, &shader_module_create_info, nullptr, &module));

    s_shader_desc *shader = (s_shader_desc*) g_resource_pool.alloc(1);
    shader->module = module;
    shader->entry = entry;
    switch (shader_type) {
        case e_shader_type::vertex:
            shader->stage_flag = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case e_shader_type::fragment:
            shader->stage_flag = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
    }

    (*handle) = reinterpret_cast<shander_handle>(shader);
    return true;
}

void get_shader_stage_info(shander_handle handle, VkPipelineShaderStageCreateInfo& info) {
    if (handle == INVALID_HANDLE) {
        return;
    }
    s_shader_desc* desc = reinterpret_cast<s_shader_desc*>(handle);
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = desc->stage_flag;
    info.module = desc->module;
    info.pName = desc->entry;
}

void destroy(s_shader_desc* desc) {
    vkDestroyShaderModule(vk_globals::device, desc->module, nullptr);
}


void destroy_shader(shander_handle handle) {
    if (handle == INVALID_HANDLE) {
        return;
    }
    s_shader_desc* desc = reinterpret_cast<s_shader_desc*>(handle);
    destroy(desc);
    g_resource_pool.free(desc);
}

}
}
}
}
