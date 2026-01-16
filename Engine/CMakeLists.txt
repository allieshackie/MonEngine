# MonEngine CMakeLists.txt - Standalone Engine Build

cmake_minimum_required(VERSION 3.18.2)

# Set up project if this is the top-level CMakeLists.txt (submodule usage)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    project(MonEngine)

    # Set up vcpkg for standalone builds
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
        set(VCPKG_MANIFEST_MODE ON)
    endif()
endif()

# Include CMake utilities if available (for non-standalone builds)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CmakeUtils.cmake")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CmakeUtils.cmake)
endif()

# Define LLGL utility
add_definitions("-DLLGL_ENABLE_UTILITY")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build shared libs by default for submodule usage
if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

# Set output directories
if(MONENGINE_STANDALONE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES "Source/*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "Source Files/" FILES ${SOURCES})

add_library(MonEngine ${SOURCES})

# Set folder property if in a larger project
if(NOT MONENGINE_STANDALONE)
    set_property(TARGET MonEngine PROPERTY VS_FOLDER "")
endif()

# Build GLAD
set(GLAD_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Lib/glad/src/glad.c
)
add_library(glad ${GLAD_SRC})

if(NOT MONENGINE_STANDALONE)
    set_property(TARGET glad PROPERTY FOLDER "ThirdParty")
endif()

target_include_directories(glad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Lib/glad/include>
    $<INSTALL_INTERFACE:include>
)

# Precompiled headers
target_precompile_headers(
    MonEngine
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <istream>
    <unordered_map>
    <functional>
)

# Include directories
target_include_directories(
    MonEngine
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
    $<INSTALL_INTERFACE:include>
)

# Link core dependencies
target_link_libraries(
    MonEngine
    PUBLIC
    glad
)

# Build IMGUI
set(IMGUI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui/backends/imgui_impl_win32.cpp
)
add_library(imgui ${IMGUI_SRC})

if(NOT MONENGINE_STANDALONE)
    set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")
endif()

target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/imgui>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(MonEngine PUBLIC imgui)

# Build TINYGLTF
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/tinygltf ${CMAKE_BINARY_DIR}/ThirdParty/tinygltf)
set(TINYGLTF_TARGETS
    tinygltf
    loader_example
)
if(NOT MONENGINE_STANDALONE)
    set_target_folder_prop(TINYGLTF_TARGETS ThirdParty)
endif()

target_link_libraries(MonEngine PUBLIC tinygltf)

# Find and link external dependencies
find_package(entt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(lua CONFIG REQUIRED)
find_package(bullet CONFIG REQUIRED)

target_link_libraries(MonEngine PUBLIC
    EnTT::EnTT
    nlohmann_json::nlohmann_json
    glm::glm
    lua
    LinearMath
    Bullet3Common
    BulletInverseDynamics
    BulletCollision
    BulletDynamics
    BulletSoftBody
)

# Build LLGL
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/LLGL ${CMAKE_BINARY_DIR}/ThirdParty/LLGL)
set(LLGL_TARGETS
    LLGL
    LLGL_Direct3D11
    LLGL_Null
    LLGL_OpenGL
)
if(NOT MONENGINE_STANDALONE)
    set_target_folder_prop(LLGL_TARGETS LLGL)
endif()
target_link_libraries(MonEngine PUBLIC LLGL)

# Copy data if building standalone
if(MONENGINE_STANDALONE)
    if(COMMAND copy_data_contents)
        copy_data_contents(${CMAKE_CURRENT_SOURCE_DIR}/Data/ ${CMAKE_BINARY_DIR}/Data)
    endif()
endif()

# Export the library for easy inclusion (only for standalone builds)
if(MONENGINE_STANDALONE)
    include(GNUInstallDirs)
    install(TARGETS MonEngine glad imgui ${TINYGLTF_TARGETS} ${LLGL_TARGETS}
        EXPORT MonEngineTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY Source/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    install(DIRECTORY Lib/glad/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    install(DIRECTORY ../ThirdParty/imgui/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    install(EXPORT MonEngineTargets
        FILE MonEngineTargets.cmake
        NAMESPACE MonEngine::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MonEngine
    )

    # Create a config file for find_package support
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/MonEngineConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/MonEngineConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MonEngine
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/MonEngineConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MonEngine
    )
endif()

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64
