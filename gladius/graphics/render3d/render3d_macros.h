//
// Created by alexa on 025 25 12 2016.
//

#ifndef GLADIUS_RENDER3D_MACROS_H
#define GLADIUS_RENDER3D_MACROS_H

#define MEMORY_ALLOCATE_INFO(SIZE, MEMORY_TYPE_INDEX)           \
{                                                               \
VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                         \
nullptr,                                                        \
SIZE,                                                           \
MEMORY_TYPE_INDEX                                               \
}

#define VK_ASSERT(r) { VkResult result = (r); if (result != VK_SUCCESS) { SET_ERROR(LOG_TYPE, "%s", debug::error_text(result)); assert(debug::error_text(result)); } }
#define VK_VERIFY(r) { VkResult result = (r); if (result != VK_SUCCESS) { SET_ERROR(LOG_TYPE, "%s", debug::error_text(result)); return false; } }
#define VK_VERIFY_RETURN(r, ret) { VkResult result = (r); if (result != VK_SUCCESS) { SET_ERROR(LOG_TYPE, "%s", debug::error_text(result)); return ret; } }

#endif //GLADIUS_RENDER3D_MACROS_H
