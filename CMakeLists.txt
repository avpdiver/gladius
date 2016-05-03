cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -Werror=return-type")
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


################################################################################
# GLadius                                                                   #
################################################################################

set(
        SOURCE_FILES

        gladius/core/platform.h
        gladius/core/types.h

        gladius/core/logging/logging.h
        gladius/core/logging/logging.cpp

        gladius/core/memory/allocator.h
        gladius/core/memory/default_policies.h

        gladius/core/collections/aligned_allocator.h

        gladius/core/threading/job_executor.h
        gladius/core/threading/job_executor.cpp
        gladius/core/threading/job.h
        gladius/core/threading/steal_queue.h

        gladius/core/filesystem/filesystem.cpp
        gladius/core/filesystem/filesystem.h
        gladius/core/filesystem/file.h
        gladius/core/filesystem/disk_file.cpp
        gladius/core/filesystem/disk_file.h

        gladius/core/window/window.h
        gladius/core/window/window.cpp

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
        gladius/graphics/render3d/render3d_command_buffer.h
        gladius/graphics/render3d/render3d_command_buffer.cpp
        gladius/graphics/render3d/render3d_render_pass.h
        gladius/graphics/render3d/render3d_render_pass.cpp
        gladius/graphics/render3d/render3d_texture.cpp
        gladius/graphics/render3d/render3d_texture.h

        main.cpp)

if (LINUX)
    set(_SYS pthread dl atomic xcb)
    set(_VULKAN_LIB vulkan)
elseif (WINDOWS)
    set(_SYS mingw32 version Imm32 Winmm)
    include_directories($ENV{VULKAN_HOME}/Include/)
    link_directories($ENV{VULKAN_HOME}/Bin/)
    set(_VULKAN_LIB vulkan-1)
else ()
    message(FATAL_ERROR "Unhandled case")
endif ()

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/gli"
)

add_executable(gladius ${SOURCE_FILES})
target_link_libraries(gladius ${_SYS} ${_VULKAN_LIB})