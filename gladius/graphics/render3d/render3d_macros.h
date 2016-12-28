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


#define RESOURCE_POOL(TYPE, SIZE) \
typedef typename std::aligned_storage<sizeof(TYPE), alignof(TYPE)>::type TYPE##_resource_t; \
static core::memory::c_allocator<std::array<TYPE##_resource_t, SIZE>, core::memory::c_lockfree_alloc<sizeof(TYPE##_resource_t), alignof(TYPE##_resource_t)>> g_resource_pool

#endif //GLADIUS_RENDER3D_MACROS_H
