cmake_minimum_required(VERSION 3.2)
project(gladius)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

################################################################################
# Platforms                                                                    #
################################################################################

if (WIN32)
    if (NOT WINDOWS)
        set(WINDOWS TRUE)
        message(STATUS "++ Building for windows")
    endif ()
elseif (UNIX AND NOT APPLE)
    if (CMAKE_SYSTEM_NAME MATCHES ".*Linux")
        if (NOT ANDROID)
            set(LINUX TRUE)
            message(STATUS "++ Building for Linux")
        else ()
            message(STATUS "++ Building for Android")
        endif ()
    else ()
        message(FATAL_ERROR "Unknown unix")
    endif ()
elseif (APPLE)
    if (CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
        set(MACOS TRUE)
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
        PREFIX "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sdl2"
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3dparty/SDL2"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sdl2 -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_BUILD_TYPE=Release -DDIRECTX=OFF -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})

ExternalProject_Get_Property(SDL2_PROJECT install_dir)
set(SDL2_INSTALL_DIR ${install_dir})

add_library(SDL2_LIB STATIC IMPORTED)
set_property(TARGET SDL2_LIB PROPERTY IMPORTED_LOCATION ${SDL2_INSTALL_DIR}/lib/libSDL2.a)
add_dependencies(SDL2_LIB SDL2_PROJECT)

################################################################################
# GLadius                                                                   #
################################################################################

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

        core/window/window.h
        core/window/window.cpp

        input/user_input.h
        input/user_input.cpp

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
        geometry/visibility.cpp
        geometry/transform_node.cpp
        geometry/transform_node.h

        graphics/render_context.cpp
        graphics/render_context.h

        main.cpp
)

include_directories("${SDL2_INSTALL_DIR}/include/")
link_directories(${SDL2_INSTALL_DIR}/lib)

add_executable(gladius ${SOURCE_FILES})
target_link_libraries(gladius SDL2_LIB -ldl -lpthread)