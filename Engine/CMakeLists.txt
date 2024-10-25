# Generate Solution -> cmake -B ./Solution

# set cmake version
cmake_minimum_required(VERSION 3.18.2)
include(${CMAKE_UTILS}/CmakeUtils.cmake)

ADD_DEFINE(LLGL_ENABLE_UTILITY)

# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES "Source/*")
source_group(TREE ${PROJECT_SOURCE_DIR}/Engine/Source PREFIX "Source Files/" FILES ${SOURCES})

add_library(MonEngine ${SOURCES})

set_property(TARGET MonEngine PROPERTY VS_FOLDER "")

set(GLAD_SRC 
    ${PROJECT_SOURCE_DIR}/Engine/Lib/glad/src/glad.c
)
add_library(glad ${GLAD_SRC})

set_property(TARGET glad PROPERTY FOLDER "ThirdParty")

target_include_directories(glad PUBLIC 
    ${PROJECT_SOURCE_DIR}/Engine/Lib/glad/include
)

copy_data_contents(${CMAKE_CURRENT_SOURCE_DIR}/Data/ ${CMAKE_BINARY_DIR}/Data)

target_precompile_headers(
    MonEngine 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <unordered_map>
    <functional>
)

set (BULLET_LIBRARIES    "LinearMath;Bullet3Common;BulletInverseDynamics;BulletCollision;BulletDynamics;BulletSoftBody")
target_link_libraries(
    MonEngine 
    PUBLIC
    EnTT
    nlohmann_json::nlohmann_json
    glm::glm
    LLGL
    imgui
    glad
    lua
    tinygltf
    ${BULLET_LIBRARIES}
)

message("CMAKE_CURRENT_SOURCE_DIR MonEngine: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(
    MonEngine 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${THIRD_PARTY_DIR}/bullet3/src
    ${THIRD_PARTY_DIR}/cereal/include/
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64