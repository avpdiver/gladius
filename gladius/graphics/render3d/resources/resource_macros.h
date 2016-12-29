//
// Created by alexa on 029 29 12 2016.
//

#ifndef GLADIUS_RESOURCE_MACROS_H
#define GLADIUS_RESOURCE_MACROS_H

#define RESOURCE_POOL(TYPE, SIZE) \
typedef typename std::aligned_storage<sizeof(TYPE), alignof(TYPE)>::type TYPE##_resource_t; \
static core::memory::c_allocator<std::array<TYPE##_resource_t, SIZE>, core::memory::c_lockfree_alloc<sizeof(TYPE##_resource_t), alignof(TYPE##_resource_t)>> g_resource_pool


#endif //GLADIUS_RESOURCE_MACROS_H
