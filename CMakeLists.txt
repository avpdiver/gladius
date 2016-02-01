cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Threads REQUIRED)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Failed to find Threads")
endif (NOT Threads_FOUND)

if (UNIX)
    find_package(X11 REQUIRED)
    if (NOT X11_FOUND)
        message(FATAL_ERROR "Failed to find X11")
    endif (NOT X11_FOUND)
endif (UNIX)

set(
        SOURCE_FILES

        core/platform.h
        core/types.h

        core/memory/allocators/free_list_allocator.h
        core/memory/allocators/malloc_allocator.cpp
        core/memory/allocators/malloc_allocator.h
        core/memory/regions/stack_region.h
        core/memory/regions/heap_region.h
        core/memory/allocators/sized_allocator.h

        core/collections/aligned_allocator.h

        core/resources/resource_pool.cpp
        core/resources/resource_pool.h

        core/threading/job_executor.h
        core/threading/job_executor.cpp
        core/threading/job.h
        core/threading/steal_queue.h

        core/filesystem/filesystem.cpp
        core/filesystem/filesystem.h
        core/filesystem/file.h
        core/filesystem/disk_file.cpp
        core/filesystem/disk_file.h

        core/window/nix/nix_window.cpp
        core/window/window.h
        core/window/windows/win_window.cpp
        core/window/window.cpp

        input/user_input.h
        input/nix/nix_user_input.cpp
        input/win/win_user_input.cpp

        core/math/box2.h
        core/math/box3.h
        core/math/common_math.h
        core/math/half.cpp
        core/math/half.h
        core/math/mat2.h
        core/math/mat3.h
        core/math/mat4.h
        core/math/quat.h
        core/math/vec2.h
        core/math/vec3.h
        core/math/vec4.h

        geometry/visibility.h

        geometry/space/tree_node.h
        geometry/space/tree_node.cpp
        geometry/space/quad_tree.cpp
        geometry/space/quad_tree.h
        geometry/space/deformation.cpp
        geometry/space/deformation.h
        geometry/visibility.cpp
        geometry/transform_node.cpp
        geometry/transform_node.h

        graphics/render_context.cpp
        graphics/render_context.h

        main.cpp geometry/space/deforms/spherical_deformation.cpp geometry/space/deforms/spherical_deformation.h)

add_executable(gladius ${SOURCE_FILES})

target_link_libraries(gladius ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES} ${X11_Xi_LIB} dinput8 dxguid)