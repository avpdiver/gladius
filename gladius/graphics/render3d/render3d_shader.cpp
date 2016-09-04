//
// Created by alexa on 004 04 09 2016.
//

#include <vector>
#include "../../core/filesystem/filesystem.h"
#include "render3d_globals.h"
#include "render3d_shader.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace resources {

VkShaderModule load_shader(const char *filename, e_shader_type shader_type, const char *entry,
                           VkPipelineShaderStageCreateInfo *create_info) {
    std::vector<char> code;
    if (!core::filesystem::read("disk", filename, code)) {
        return nullptr;
    }

    VkShaderModuleCreateInfo shader_module_create_info = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
            nullptr,                                        // const void                    *pNext
            0,                                              // VkShaderModuleCreateFlags      flags
            code.size(),                                    // size_t                         codeSize
            reinterpret_cast<const uint32_t *>(&code[0])     // const uint32_t                *pCode
    };

    VkShaderModule shader_module;
    VK_VERIFY_RETURN(vkCreateShaderModule(vk_globals::device, &shader_module_create_info, nullptr,
                                          &shader_module), nullptr);

    if (entry != nullptr && create_info != nullptr) {
        VkShaderStageFlagBits flag_bits;
        switch (shader_type) {
            case e_shader_type::vertex:
                flag_bits = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case e_shader_type::fragment:
                flag_bits = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
        }

        *(create_info) = {
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
                nullptr,                                                    // const void                                    *pNext
                0,                                                          // VkPipelineShaderStageCreateFlags               flags
                flag_bits,                                                  // VkShaderStageFlagBits                          stage
                shader_module,                                              // VkShaderModule                                 module
                entry,                                                      // const char                                    *pName
                nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
        };
    }

    return shader_module;
}

void destroy(VkShaderModule shader) {
    vkDestroyShaderModule(vk_globals::device, shader, nullptr);
}

}
}
}
}
