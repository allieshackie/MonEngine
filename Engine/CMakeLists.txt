# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
source_group(TREE ${PROJECT_SOURCE_DIR}/Engine/Source PREFIX "Source Files/" FILES ${SOURCES})

add_library(Engine ${SOURCES})

set_property(TARGET Engine PROPERTY VS_FOLDER "")
add_library(MonDev::Engine ALIAS Engine)

set(GLAD_SRC 
    ${PROJECT_SOURCE_DIR}/Engine/Lib/glad/src/glad.c
)
add_library(glad ${GLAD_SRC})

set_property(TARGET glad PROPERTY FOLDER "ThirdParty")

target_include_directories(glad PUBLIC 
    ${PROJECT_SOURCE_DIR}/Engine/Lib/glad/include
)

copy_runtime_data(Engine "${CMAKE_CURRENT_SOURCE_DIR}/Data" engine)

target_compile_features(Engine PUBLIC cxx_std_17)
set_target_properties(Engine PROPERTIES CXX_EXTENSIONS OFF)
target_compile_definitions(Engine PRIVATE
    GLM_ENABLE_EXPERIMENTAL
    LLGL_ENABLE_UTILITY
    ASSETS_FOLDER="${CMAKE_BINARY_DIR}/Data/Assets/"
    LEVELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Levels/"
    PREFABS_FOLDER="${CMAKE_BINARY_DIR}/Data/Prefabs/"
    SHADERS_FOLDER="${CMAKE_BINARY_DIR}/Data/Shaders/"
    SCRIPTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Scripts/"
)

target_precompile_headers(
    Engine 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <istream>
    <unordered_map>
    <functional>
)

target_link_libraries(
    Engine 
    PUBLIC
    imgui
    tinygltf
)
target_include_directories(
    Engine 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64
