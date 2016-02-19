cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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

        core/platform.h
        core/types.h

        core/logging/logging.h
        core/logging/logging.cpp

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

        core/window/window.h
        core/window/window.cpp

        core/math/aabb.h

        input/user_input.h
        input/user_input.cpp

        geometry/visibility.h

        graphics/renderer3d.h
        graphics/renderer3d.cpp
        graphics/rendered3d_debugger.cpp
        graphics/render_context.cpp
        graphics/render_context.h

        main.cpp
)

if (LINUX)
    set(_SYS pthread dl)
elseif (MACOS)
    find_package(OpenGL REQUIRED)
    set(_SYS ${OPENGL_LIBRARIES} pthread)
elseif (ANDROID)
    set(_SYS GLESv3 EGL log android)
    include_directories("${ANDROID_NDK}/sources/android/native_app_glue")
    set(_SYS_SRC "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
elseif (WINDOWS)
    set(_SYS opengl32 mingw32 version Imm32 Winmm)
    add_definitions(-DGLEW_STATIC -D_NEWTON_STATIC_LIB)
else ()
    message(FATAL_ERROR "Unhandled case")
endif ()

include_directories(
        "${SDL2_INSTALL_DIR}/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm"
)

link_directories(${SDL2_INSTALL_DIR}/lib/)

add_executable(gladius ${SOURCE_FILES})
add_dependencies(gladius SDL2_LIB SDL2MAIN_LIB)
target_link_libraries(gladius SDL2MAIN_LIB SDL2_LIB ${_SYS} libvulkan.so)