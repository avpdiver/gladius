//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_UTILS_H
#define GLADIUS_RENDER3D_UTILS_H

#include <string>
#include <vulkan/vulkan.h>

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

bool check_extension(const char *extension_name,
					 const std::vector<VkExtensionProperties> &available_extensions);

VkSampleCountFlagBits get_sample_count(const size_t &samples);

VkFormat string_to_format(const std::string &str);

VkPolygonMode string_to_polygon_mode(const std::string &str);

VkCullModeFlags string_to_cull_mode(const std::string &str);

VkFrontFace string_to_front_face(const std::string &str);

VkBlendFactor string_to_blend_color(const std::string &str);

VkBlendOp string_to_blend_op(const std::string &str);

VkColorComponentFlags string_to_color_component_flag(const std::string &str);

VkLogicOp string_to_logic_op(const std::string &str);

VkAttachmentLoadOp string_to_load_op(const std::string &str);

VkAttachmentStoreOp string_to_store_op(const std::string &str);

VkImageLayout string_to_image_layout(const std::string &str);

VkPipelineBindPoint string_to_bind_point(const std::string &str);

}
}
}
}

#endif //GLADIUS_RENDER3D_UTILS_H
