cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -pedantic -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_CURRENT_SOURCE_DIR}/,,$(abspath $<))\"'")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

################################################################################
# Platforms                                                                    #
################################################################################

if (WIN32)
    if (NOT WINDOWS)
        set(WINDOWS TRUE)
        add_definitions(-DPLATFORM_WINDOWS)
        message(STATUS "++ Building for windows")
    endif ()
elseif (UNIX AND NOT APPLE)
    if (CMAKE_SYSTEM_NAME MATCHES ".*Linux")
        if (NOT ANDROID)
            set(LINUX TRUE)
            add_definitions(-DPLATFORM_LINUX)
            message(STATUS "++ Building for Linux")
        else ()
            set(ANDROID TRUE)
            add_definitions(-DPLATFORM_ANDROID)
            message(STATUS "++ Building for Android")
        endif ()
    else ()
        message(FATAL_ERROR "Unknown unix")
    endif ()
elseif (APPLE)
    if (CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
        set(MACOS TRUE)
        add_definitions(-DPLATFORM_MACOS)
        message(STATUS "++ Building for MacOS")
    else ()
        message(FATAL_ERROR "Unknown apple")
    endif ()
else ()
    message(FATAL_ERROR "Unknown system")
endif ()

################################################################################
# Libraries                                                                    #
################################################################################

if ("$ENV{VULKAN_SDK}" STREQUAL "")
    message(FATAL_ERROR "You need to have system-wide variable with path to Vulkan SDK")
else()
    message(STATUS "Vulkan SDK path: " $ENV{VULKAN_SDK})
endif()

if (LINUX)
    set(_SYS pthread dl atomic xcb supc++)
    set(_VULKAN_LIB vulkan)
elseif (WINDOWS)
    set(_SYS mingw32 version Imm32 Winmm Ws2_32 Mswsock AdvApi32)
    set(_VULKAN_LIB vulkan-1)
else ()
    message(FATAL_ERROR "Unhandled case")
endif ()

################################################################################
# GLadius                                                                   #
################################################################################

set(
        SOURCE_FILES

        gladius/core/platform.h
        gladius/core/types.h
        gladius/core/json.h

        gladius/core/meta/fixed_function.h

        gladius/core/logger/logger.h
        gladius/core/logger/logger.cpp

        gladius/core/memory/memory_utils.h
        gladius/core/memory/allocator.h
        gladius/core/memory/default_policies.h

        gladius/core/collections/aligned_allocator.h
        gladius/core/collections/steal_queue.h
        gladius/core/collections/concurrent_queue.h

        gladius/core/threading/job.h
        gladius/core/threading/worker_pool.h
        gladius/core/threading/worker.h
        gladius/core/threading/weak_semaphore.h
        gladius/core/threading/movable_atomic.h

        gladius/core/filesystem/filesystem.cpp
        gladius/core/filesystem/filesystem.h
        gladius/core/filesystem/file.h
        gladius/core/filesystem/disk_file.cpp
        gladius/core/filesystem/disk_file.h
        gladius/core/filesystem/json_file.cpp
        gladius/core/filesystem/json_file.h

        gladius/core/window/window.h
        gladius/core/window/window.cpp
        gladius/core/window/window_win.cpp
        gladius/core/window/window_xcb.cpp

        gladius/core/math/math_func.h
        gladius/core/math/aabb.h

        gladius/input/user_input.h
        gladius/input/user_input.cpp

        gladius/geometry/visibility.h

        gladius/graphics/render3d/render3d.h
        gladius/graphics/render3d/render3d_utils.h
        gladius/graphics/render3d/render3d_utils.cpp
        gladius/graphics/render3d/render3d_globals.h
        gladius/graphics/render3d/render3d.cpp
        gladius/graphics/render3d/render3d_debug.h
        gladius/graphics/render3d/render3d_debug.cpp
        gladius/graphics/render3d/render3d_device.cpp
        gladius/graphics/render3d/render3d_device.h
        gladius/graphics/render3d/render3d_swapchain.cpp
        gladius/graphics/render3d/render3d_swapchain.h
        gladius/graphics/render3d/render3d_resources.cpp
        gladius/graphics/render3d/render3d_resources.h
        gladius/graphics/render3d/render3d_buffer.cpp
        gladius/graphics/render3d/render3d_buffer.h
        gladius/graphics/render3d/render3d_command_buffer.h
        gladius/graphics/render3d/render3d_command_buffer.cpp
        gladius/graphics/render3d/render3d_shader.cpp
        gladius/graphics/render3d/render3d_shader.h
        gladius/graphics/render3d/render3d_render_pass.h
        gladius/graphics/render3d/render3d_render_pass.cpp
        gladius/graphics/render3d/render3d_texture.cpp
        gladius/graphics/render3d/render3d_texture.h
        gladius/graphics/render3d/render3d_pipeline.cpp
        gladius/graphics/render3d/render3d_pipeline.h

        gladius/ecs/system.h

        gladius/gladius.h
        gladius/gladius.cpp

        main.cpp gladius/ecs/mt/single_thread_executor.h gladius/ecs/mt/multithread_executor.h)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/gli"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/json/src"
        $ENV{VULKAN_SDK}/include/
)

link_directories($ENV{VULKAN_SDK}/lib/)

add_executable(gladius ${SOURCE_FILES})
target_link_libraries(gladius ${_SYS} ${_VULKAN_LIB})