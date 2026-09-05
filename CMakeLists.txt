cmake_minimum_required(VERSION 3.25...4.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "CMake toolchain file"
    )
endif()

project(MonDev LANGUAGES C CXX)

set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/ThirdParty")
set(CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("${CMAKE_UTILS}/CmakeUtils.cmake")

set(BUILD_SHARED_LIBS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeGenerated")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(PROJ_TO_BUILD "engine" CACHE STRING "Application target to configure: engine, game, or sandbox")
set_property(CACHE PROJ_TO_BUILD PROPERTY STRINGS engine game sandbox)

# Only build the third-party targets used by MonDev.
set(LLGL_BUILD_RENDERER_NULL OFF CACHE BOOL "" FORCE)
set(LLGL_BUILD_RENDERER_DIRECT3D11 OFF CACHE BOOL "" FORCE)
set(LLGL_BUILD_RENDERER_OPENGL ON CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE BOOL "" FORCE)

message(STATUS "Building Engine")
add_subdirectory("${PROJECT_SOURCE_DIR}/Engine")

if(PROJ_TO_BUILD STREQUAL "sandbox")
    message(STATUS "Building Sandbox")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Sandbox")
    set(PROJ_NAME Sandbox)
elseif(PROJ_TO_BUILD STREQUAL "game")
    message(STATUS "Building Game")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Game")
    set(PROJ_NAME Game)
elseif(PROJ_TO_BUILD STREQUAL "engine")
    message(STATUS "Building Engine standalone")
    set(PROJ_NAME Engine)
else()
    message(FATAL_ERROR "Invalid PROJ_TO_BUILD='${PROJ_TO_BUILD}'. Expected engine, game, or sandbox.")
endif()

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJ_NAME}")

message(STATUS "Finding EnTT")
find_package(entt CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC EnTT::EnTT)

message(STATUS "Finding nlohmann-json")
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC nlohmann_json::nlohmann_json)

message(STATUS "Finding GLM")
find_package(glm CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC glm::glm glad)

message(STATUS "Building LLGL")
add_subdirectory("${THIRD_PARTY_DIR}/LLGL")
set(LLGL_TARGETS LLGL LLGL_OpenGL)
set_target_folder_prop(LLGL_TARGETS LLGL)
target_link_libraries(Engine PUBLIC LLGL)

message(STATUS "Building ImGui")
set(IMGUI_SRC
    "${THIRD_PARTY_DIR}/imgui/imgui.cpp"
    "${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp"
    "${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp"
    "${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp"
    "${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp"
    "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.cpp"
)
add_library(imgui ${IMGUI_SRC})
set_property(TARGET imgui PROPERTY FOLDER ThirdParty)
target_include_directories(imgui PUBLIC "${THIRD_PARTY_DIR}/imgui")

message(STATUS "Building tinygltf")
add_subdirectory("${THIRD_PARTY_DIR}/tinygltf")
set(TINYGLTF_TARGETS tinygltf)
set_target_folder_prop(TINYGLTF_TARGETS ThirdParty)

message(STATUS "Finding Lua")
find_package(lua CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC lua)

message(STATUS "Finding Bullet")
find_package(bullet CONFIG REQUIRED)
set(BULLET_LIBRARIES
    LinearMath
    Bullet3Common
    BulletInverseDynamics
    BulletCollision
    BulletDynamics
    BulletSoftBody
)
target_link_libraries(Engine PUBLIC ${BULLET_LIBRARIES})

message(STATUS "Finding Cereal")
find_package(cereal CONFIG REQUIRED)
target_link_libraries(Engine PUBLIC cereal::cereal)
