//
// Created by alexa on 022 22 02 2016.
//

#include <map>
#include <cstring>
#include "render3d_globals.h"
#include "render3d_utils.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace utils {

#define IF(S, SUFFIX, PREFIX)  if (S == #SUFFIX) return PREFIX##_##SUFFIX

#define FORMAT(F) { #F, VK_FORMAT_##F }

std::map<const std::string, VkFormat> g_formats_map = {
        FORMAT(UNDEFINED),
        FORMAT(R4G4_UNORM_PACK8),
        FORMAT(R4G4B4A4_UNORM_PACK16),
        FORMAT(B4G4R4A4_UNORM_PACK16),
        FORMAT(R5G6B5_UNORM_PACK16),
        FORMAT(B5G6R5_UNORM_PACK16),
        FORMAT(R5G5B5A1_UNORM_PACK16),
        FORMAT(B5G5R5A1_UNORM_PACK16),
        FORMAT(A1R5G5B5_UNORM_PACK16),
        FORMAT(R8_UNORM),
        FORMAT(R8_SNORM),
        FORMAT(R8_USCALED),
        FORMAT(R8_SSCALED),
        FORMAT(R8_UINT),
        FORMAT(R8_SINT),
        FORMAT(R8_SRGB),
        FORMAT(R8G8_UNORM),
        FORMAT(R8G8_SNORM),
        FORMAT(R8G8_USCALED),
        FORMAT(R8G8_SSCALED),
        FORMAT(R8G8_UINT),
        FORMAT(R8G8_SINT),
        FORMAT(R8G8_SRGB),
        FORMAT(R8G8B8_UNORM),
        FORMAT(R8G8B8_SNORM),
        FORMAT(R8G8B8_USCALED),
        FORMAT(R8G8B8_SSCALED),
        FORMAT(R8G8B8_UINT),
        FORMAT(R8G8B8_SINT),
        FORMAT(R8G8B8_SRGB),
        FORMAT(B8G8R8_UNORM),
        FORMAT(B8G8R8_SNORM),
        FORMAT(B8G8R8_USCALED),
        FORMAT(B8G8R8_SSCALED),
        FORMAT(B8G8R8_UINT),
        FORMAT(B8G8R8_SINT),
        FORMAT(B8G8R8_SRGB),
        FORMAT(R8G8B8A8_UNORM),
        FORMAT(R8G8B8A8_SNORM),
        FORMAT(R8G8B8A8_USCALED),
        FORMAT(R8G8B8A8_SSCALED),
        FORMAT(R8G8B8A8_UINT),
        FORMAT(R8G8B8A8_SINT),
        FORMAT(R8G8B8A8_SRGB),
        FORMAT(B8G8R8A8_UNORM),
        FORMAT(B8G8R8A8_SNORM),
        FORMAT(B8G8R8A8_USCALED),
        FORMAT(B8G8R8A8_SSCALED),
        FORMAT(B8G8R8A8_UINT),
        FORMAT(B8G8R8A8_SINT),
        FORMAT(B8G8R8A8_SRGB),
        FORMAT(A8B8G8R8_UNORM_PACK32),
        FORMAT(A8B8G8R8_SNORM_PACK32),
        FORMAT(A8B8G8R8_USCALED_PACK32),
        FORMAT(A8B8G8R8_SSCALED_PACK32),
        FORMAT(A8B8G8R8_UINT_PACK32),
        FORMAT(A8B8G8R8_SINT_PACK32),
        FORMAT(A8B8G8R8_SRGB_PACK32),
        FORMAT(A2R10G10B10_UNORM_PACK32),
        FORMAT(A2R10G10B10_SNORM_PACK32),
        FORMAT(A2R10G10B10_USCALED_PACK32),
        FORMAT(A2R10G10B10_SSCALED_PACK32),
        FORMAT(A2R10G10B10_UINT_PACK32),
        FORMAT(A2R10G10B10_SINT_PACK32),
        FORMAT(A2B10G10R10_UNORM_PACK32),
        FORMAT(A2B10G10R10_SNORM_PACK32),
        FORMAT(A2B10G10R10_USCALED_PACK32),
        FORMAT(A2B10G10R10_SSCALED_PACK32),
        FORMAT(A2B10G10R10_UINT_PACK32),
        FORMAT(A2B10G10R10_SINT_PACK32),
        FORMAT(R16_UNORM),
        FORMAT(R16_SNORM),
        FORMAT(R16_USCALED),
        FORMAT(R16_SSCALED),
        FORMAT(R16_UINT),
        FORMAT(R16_SINT),
        FORMAT(R16_SFLOAT),
        FORMAT(R16G16_UNORM),
        FORMAT(R16G16_SNORM),
        FORMAT(R16G16_USCALED),
        FORMAT(R16G16_SSCALED),
        FORMAT(R16G16_UINT),
        FORMAT(R16G16_SINT),
        FORMAT(R16G16_SFLOAT),
        FORMAT(R16G16B16_UNORM),
        FORMAT(R16G16B16_SNORM),
        FORMAT(R16G16B16_USCALED),
        FORMAT(R16G16B16_SSCALED),
        FORMAT(R16G16B16_UINT),
        FORMAT(R16G16B16_SINT),
        FORMAT(R16G16B16_SFLOAT),
        FORMAT(R16G16B16A16_UNORM),
        FORMAT(R16G16B16A16_SNORM),
        FORMAT(R16G16B16A16_USCALED),
        FORMAT(R16G16B16A16_SSCALED),
        FORMAT(R16G16B16A16_UINT),
        FORMAT(R16G16B16A16_SINT),
        FORMAT(R16G16B16A16_SFLOAT),
        FORMAT(R32_UINT),
        FORMAT(R32_SINT),
        FORMAT(R32_SFLOAT),
        FORMAT(R32G32_UINT),
        FORMAT(R32G32_SINT),
        FORMAT(R32G32_SFLOAT),
        FORMAT(R32G32B32_UINT),
        FORMAT(R32G32B32_SINT),
        FORMAT(R32G32B32_SFLOAT),
        FORMAT(R32G32B32A32_UINT),
        FORMAT(R32G32B32A32_SINT),
        FORMAT(R32G32B32A32_SFLOAT),
        FORMAT(R64_UINT),
        FORMAT(R64_SINT),
        FORMAT(R64_SFLOAT),
        FORMAT(R64G64_UINT),
        FORMAT(R64G64_SINT),
        FORMAT(R64G64_SFLOAT),
        FORMAT(R64G64B64_UINT),
        FORMAT(R64G64B64_SINT),
        FORMAT(R64G64B64_SFLOAT),
        FORMAT(R64G64B64A64_UINT),
        FORMAT(R64G64B64A64_SINT),
        FORMAT(R64G64B64A64_SFLOAT),
        FORMAT(B10G11R11_UFLOAT_PACK32),
        FORMAT(E5B9G9R9_UFLOAT_PACK32),
        FORMAT(D16_UNORM),
        FORMAT(X8_D24_UNORM_PACK32),
        FORMAT(D32_SFLOAT),
        FORMAT(S8_UINT),
        FORMAT(D16_UNORM_S8_UINT),
        FORMAT(D24_UNORM_S8_UINT),
        FORMAT(D32_SFLOAT_S8_UINT),
        FORMAT(BC1_RGB_UNORM_BLOCK),
        FORMAT(BC1_RGB_SRGB_BLOCK),
        FORMAT(BC1_RGBA_UNORM_BLOCK),
        FORMAT(BC1_RGBA_SRGB_BLOCK),
        FORMAT(BC2_UNORM_BLOCK),
        FORMAT(BC2_SRGB_BLOCK),
        FORMAT(BC3_UNORM_BLOCK),
        FORMAT(BC3_SRGB_BLOCK),
        FORMAT(BC4_UNORM_BLOCK),
        FORMAT(BC4_SNORM_BLOCK),
        FORMAT(BC5_UNORM_BLOCK),
        FORMAT(BC5_SNORM_BLOCK),
        FORMAT(BC6H_UFLOAT_BLOCK),
        FORMAT(BC6H_SFLOAT_BLOCK),
        FORMAT(BC7_UNORM_BLOCK),
        FORMAT(BC7_SRGB_BLOCK),
        FORMAT(ETC2_R8G8B8_UNORM_BLOCK),
        FORMAT(ETC2_R8G8B8_SRGB_BLOCK),
        FORMAT(ETC2_R8G8B8A1_UNORM_BLOCK),
        FORMAT(ETC2_R8G8B8A1_SRGB_BLOCK),
        FORMAT(ETC2_R8G8B8A8_UNORM_BLOCK),
        FORMAT(ETC2_R8G8B8A8_SRGB_BLOCK),
        FORMAT(EAC_R11_UNORM_BLOCK),
        FORMAT(EAC_R11_SNORM_BLOCK),
        FORMAT(EAC_R11G11_UNORM_BLOCK),
        FORMAT(EAC_R11G11_SNORM_BLOCK),
        FORMAT(ASTC_4x4_UNORM_BLOCK),
        FORMAT(ASTC_4x4_SRGB_BLOCK),
        FORMAT(ASTC_5x4_UNORM_BLOCK),
        FORMAT(ASTC_5x4_SRGB_BLOCK),
        FORMAT(ASTC_5x5_UNORM_BLOCK),
        FORMAT(ASTC_5x5_SRGB_BLOCK),
        FORMAT(ASTC_6x5_UNORM_BLOCK),
        FORMAT(ASTC_6x5_SRGB_BLOCK),
        FORMAT(ASTC_6x6_UNORM_BLOCK),
        FORMAT(ASTC_6x6_SRGB_BLOCK),
        FORMAT(ASTC_8x5_UNORM_BLOCK),
        FORMAT(ASTC_8x5_SRGB_BLOCK),
        FORMAT(ASTC_8x6_UNORM_BLOCK),
        FORMAT(ASTC_8x6_SRGB_BLOCK),
        FORMAT(ASTC_8x8_UNORM_BLOCK),
        FORMAT(ASTC_8x8_SRGB_BLOCK),
        FORMAT(ASTC_10x5_UNORM_BLOCK),
        FORMAT(ASTC_10x5_SRGB_BLOCK),
        FORMAT(ASTC_10x6_UNORM_BLOCK),
        FORMAT(ASTC_10x6_SRGB_BLOCK),
        FORMAT(ASTC_10x8_UNORM_BLOCK),
        FORMAT(ASTC_10x8_SRGB_BLOCK),
        FORMAT(ASTC_10x10_UNORM_BLOCK),
        FORMAT(ASTC_10x10_SRGB_BLOCK),
        FORMAT(ASTC_12x10_UNORM_BLOCK),
        FORMAT(ASTC_12x10_SRGB_BLOCK),
        FORMAT(ASTC_12x12_UNORM_BLOCK),
        FORMAT(ASTC_12x12_SRGB_BLOCK)
};

bool check_extension(const char *extension_name, const std::vector<VkExtensionProperties> &available_extensions) {
    for (size_t i = 0; i < available_extensions.size(); ++i) {
        if (strcmp(available_extensions[i].extensionName, extension_name) == 0) {
            return true;
        }
    }
    return false;
}

int find_memory_type(uint32_t memory_type_bits, VkMemoryPropertyFlagBits property_flags) {
    for (uint32_t i = 0; i < vk_globals::gpu_memory_properties.memoryTypeCount; ++i) {
        if ((memory_type_bits & (1 << i))
            && (vk_globals::gpu_memory_properties.memoryTypes[i].propertyFlags & property_flags)) {
            return i;
        }
    }
    return -1;
}

VkSampleCountFlagBits get_sample_count(const size_t &samples) {
    if (samples == 1)
        return VK_SAMPLE_COUNT_1_BIT;
    if (samples == 2)
        return VK_SAMPLE_COUNT_2_BIT;
    if (samples == 4)
        return VK_SAMPLE_COUNT_4_BIT;
    if (samples == 8)
        return VK_SAMPLE_COUNT_8_BIT;
    if (samples == 16)
        return VK_SAMPLE_COUNT_16_BIT;
    if (samples == 32)
        return VK_SAMPLE_COUNT_32_BIT;
    if (samples == 64)
        return VK_SAMPLE_COUNT_64_BIT;
    return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat string_to_format(const std::string &str) {
    const auto it = g_formats_map.find(str);
    if (it == g_formats_map.cend()) {
        return VK_FORMAT_UNDEFINED;
    } else {
        return it->second;
    }
}

VkPolygonMode string_to_polygon_mode(const std::string &str) {
    IF (str, FILL, VK_POLYGON_MODE);
    IF (str, LINE, VK_POLYGON_MODE);
    IF (str, POINT, VK_POLYGON_MODE);
    return VK_POLYGON_MODE_FILL;
}

VkCullModeFlags string_to_cull_mode(const std::string &str) {
    VkCullModeFlags flag = VK_CULL_MODE_NONE;
    size_t pos = 0;
    size_t n = 0;
    while (n < str.length()) {
        n = str.find_first_of(' ', pos);
        if (n == std::string::npos) {
            n = str.length();
        }
        std::string token = str.substr(pos, n);
        if (token == "FRONT")
            flag |= VK_CULL_MODE_FRONT_BIT;
        if (token == "BACK")
            flag |= VK_CULL_MODE_BACK_BIT;
    }
    return flag;
}

VkFrontFace string_to_front_face(const std::string &str) {
    IF (str, COUNTER_CLOCKWISE, VK_FRONT_FACE);
    IF (str, CLOCKWISE, VK_FRONT_FACE);
    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

VkBlendFactor string_to_blend_color(const std::string &str) {
    IF (str, ZERO, VK_BLEND_FACTOR);
    IF (str, ONE, VK_BLEND_FACTOR);
    IF (str, SRC_COLOR, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_SRC_COLOR, VK_BLEND_FACTOR);
    IF (str, DST_COLOR, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_DST_COLOR, VK_BLEND_FACTOR);
    IF (str, SRC_ALPHA, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR);
    IF (str, DST_ALPHA, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_DST_ALPHA, VK_BLEND_FACTOR);
    IF (str, CONSTANT_COLOR, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_CONSTANT_COLOR, VK_BLEND_FACTOR);
    IF (str, CONSTANT_ALPHA, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_CONSTANT_ALPHA, VK_BLEND_FACTOR);
    IF (str, SRC_ALPHA_SATURATE, VK_BLEND_FACTOR);
    IF (str, SRC1_COLOR, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_SRC1_COLOR, VK_BLEND_FACTOR);
    IF (str, SRC1_ALPHA, VK_BLEND_FACTOR);
    IF (str, ONE_MINUS_SRC1_ALPHA, VK_BLEND_FACTOR);
    return VK_BLEND_FACTOR_ZERO;
}

VkBlendOp string_to_blend_op(const std::string &str) {
    IF (str, ADD, VK_BLEND_OP);
    IF (str, SUBTRACT, VK_BLEND_OP);
    IF (str, REVERSE_SUBTRACT, VK_BLEND_OP);
    IF (str, MIN, VK_BLEND_OP);
    IF (str, MAX, VK_BLEND_OP);
    return VK_BLEND_OP_ADD;
}

VkColorComponentFlags string_to_color_component_flag(const std::string &str) {
    VkColorComponentFlags flag = 0;
    if (str.find('R') != std::string::npos)
        flag |= VK_COLOR_COMPONENT_R_BIT;
    if (str.find('G') != std::string::npos)
        flag |= VK_COLOR_COMPONENT_G_BIT;
    if (str.find('B') != std::string::npos)
        flag |= VK_COLOR_COMPONENT_B_BIT;
    if (str.find('A') != std::string::npos)
        flag |= VK_COLOR_COMPONENT_A_BIT;
    return flag;
}

VkLogicOp string_to_logic_op(const std::string &str) {
    IF (str, CLEAR, VK_LOGIC_OP);
    IF (str, AND, VK_LOGIC_OP);
    IF (str, AND_REVERSE, VK_LOGIC_OP);
    IF (str, COPY, VK_LOGIC_OP);
    IF (str, AND_INVERTED, VK_LOGIC_OP);
    IF (str, NO_OP, VK_LOGIC_OP);
    IF (str, XOR, VK_LOGIC_OP);
    IF (str, OR, VK_LOGIC_OP);
    IF (str, NOR, VK_LOGIC_OP);
    IF (str, EQUIVALENT, VK_LOGIC_OP);
    IF (str, INVERT, VK_LOGIC_OP);
    IF (str, OR_REVERSE, VK_LOGIC_OP);
    IF (str, COPY_INVERTED, VK_LOGIC_OP);
    IF (str, OR_INVERTED, VK_LOGIC_OP);
    IF (str, NAND, VK_LOGIC_OP);
    IF (str, SET, VK_LOGIC_OP);
    return VK_LOGIC_OP_COPY;
}

VkAttachmentLoadOp string_to_load_op(const std::string &str) {
    IF (str, LOAD, VK_ATTACHMENT_LOAD_OP);
    IF (str, CLEAR, VK_ATTACHMENT_LOAD_OP);
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp string_to_store_op(const std::string &str) {
    IF (str, STORE, VK_ATTACHMENT_STORE_OP);
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkImageLayout string_to_image_layout(const std::string &str) {
    IF (str, GENERAL, VK_IMAGE_LAYOUT);
    IF (str, COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT);
    IF (str, PREINITIALIZED, VK_IMAGE_LAYOUT);
    IF (str, PRESENT_SRC_KHR, VK_IMAGE_LAYOUT);
    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkPipelineBindPoint string_to_bind_point(const std::string &str) {
    IF (str, COMPUTE, VK_PIPELINE_BIND_POINT);
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

}
}
}
}