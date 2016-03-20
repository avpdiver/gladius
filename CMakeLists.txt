cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror=return-type")
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
# Thirdparty                                                                   #
################################################################################
include(ExternalProject)

# SDL
ExternalProject_Add(
        SDL2_PROJECT
        DOWNLOAD_COMMAND ""
        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/install"
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/install -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_BUILD_TYPE=Release -DDIRECTX=OFF -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})

ExternalProject_Get_Property(SDL2_PROJECT install_dir)
set(SDL2_INSTALL_DIR ${install_dir})

add_library(SDL2_LIB STATIC IMPORTED)
set_property(TARGET SDL2_LIB PROPERTY IMPORTED_LOCATION ${SDL2_INSTALL_DIR}/lib/libSDL2.a)
add_dependencies(SDL2_LIB SDL2_PROJECT)

add_library(SDL2MAIN_LIB STATIC IMPORTED)
set_property(TARGET SDL2MAIN_LIB PROPERTY IMPORTED_LOCATION ${SDL2_INSTALL_DIR}/lib/libSDL2main.a)
add_dependencies(SDL2MAIN_LIB SDL2_PROJECT)

################################################################################
# GLadius                                                                   #
################################################################################

set(
        SOURCE_FILES

        gladius/core/platform.h
        gladius/core/types.h

        gladius/core/logging/logging.h
        gladius/core/logging/logging.cpp

        gladius/core/memory/memory_allocator.h

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

        gladius/graphics/render3d/render3d_types.h
        gladius/graphics/render3d/render3d.h
        gladius/graphics/render3d/render3d_utils.h
        gladius/graphics/render3d/render3d_utils.cpp
        gladius/graphics/render3d/render3d_globals.h
        gladius/graphics/render3d/render3d.cpp
        gladius/graphics/render3d/render3d_debug.h
        gladius/graphics/render3d/render3d_debug.cpp
        gladius/graphics/render3d/render3d_swapchain.h
        gladius/graphics/render3d/render3d_swapchain.cpp
        gladius/graphics/render3d/render3d_command_buffer.h
        gladius/graphics/render3d/render3d_command_buffer.cpp
        gladius/graphics/render3d/render3d_render_pass.h
        gladius/graphics/render3d/render3d_render_pass.cpp
        gladius/graphics/render3d/render3d_texture.cpp
        gladius/graphics/render3d/render3d_texture.h

        main.cpp
        gladius/core/memory/lockfree_allocator_policy.h)

if (LINUX)
    set(_SYS pthread dl)
elseif (MACOS)
    set(_SYS pthread)
elseif (ANDROID)
    set(_SYS log android)
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")
    set(_SYS_SRC "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
elseif (WINDOWS)
    set(_SYS mingw32 version Imm32 Winmm)
    include_directories($ENV{VULKAN_HOME}/Include/)
    link_directories($ENV{VULKAN_HOME}/Bin/)
else ()
    message(FATAL_ERROR "Unhandled case")
endif ()

include_directories(
        "${SDL2_INSTALL_DIR}/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/gli"
)

link_directories(${SDL2_INSTALL_DIR}/lib/)

add_executable(gladius ${SOURCE_FILES})
add_dependencies(gladius SDL2_LIB SDL2MAIN_LIB)
target_link_libraries(gladius SDL2MAIN_LIB SDL2_LIB ${_SYS} -lvulkan-1)