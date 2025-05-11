# Generate Solution -> cmake -B ./Build
# cmake -B ./Build -DMON_DEV_BUILD_SANDBOX=ON

cmake_minimum_required(VERSION 3.18.2)

#  ============== PROJECT ================
project(MonDev)

set(GAME_TO_BUILD "sandbox" CACHE STRING "Which game to build")

# ============== CONFIG ================

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)
set(CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(${CMAKE_UTILS}/CmakeUtils.cmake)

macro(ADD_DEFINE IDENT)
    add_definitions("-D${IDENT}")
endmacro()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeGenerated")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_compile_definitions(BUILD_GAME)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Macros must be defined before subdirectories are added
# ============== MACROS ===============
add_compile_definitions(ENTITIES_FOLDER="${CMAKE_BINARY_DIR}/Data/Entities/")
add_compile_definitions(FONTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Fonts/")
add_compile_definitions(LUA_MODULES_FOLDER="${CMAKE_BINARY_DIR}/Data/LuaModules/")
add_compile_definitions(LEVELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Levels/")
add_compile_definitions(MAPS_FOLDER="${CMAKE_BINARY_DIR}/Data/Maps/")
add_compile_definitions(MODELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Models/")
add_compile_definitions(SHADERS_FOLDER="${CMAKE_BINARY_DIR}/Data/Shaders/")
add_compile_definitions(SCRIPTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Scripts/")
add_compile_definitions(TEXTURES_FOLDER="${CMAKE_BINARY_DIR}/Data/Textures/")

# ============== ENGINE ==============
message("=== BUILDING ENGINE ===")
add_subdirectory(${PROJECT_SOURCE_DIR}/Engine)

# ====== BUILD SELECTED GAME =========
if(GAME_TO_BUILD STREQUAL "sandbox")
  message("=== BUILDING SANDBOX ===")
  add_subdirectory(${PROJECT_SOURCE_DIR}/Game)
  set(GAME_PROJ "Game")
elseif(GAME_TO_BUILD STREQUAL "dogeapp")
  message("=== BUILDING DOGEAPP ===")
  add_subdirectory(${PROJECT_SOURCE_DIR}/DogeApp)
  set(GAME_PROJ "DogeApp")
else()
  message("Unknown Game to build")
  set(GAME_PROJ "MonEngine")
endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_PROJ})

# ============== THIRD PARTY ================

## ENTT
message("=== BUILDING ENTT ===")
find_package(entt CONFIG REQUIRED)
target_link_libraries(MonEngine PUBLIC EnTT::EnTT)

## JSON
message("=== BUILDING JSON ===")
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(MonEngine PUBLIC nlohmann_json::nlohmann_json)

## GLM
message("=== BUILDING GLM ===")
find_package(glm CONFIG REQUIRED)
target_link_libraries(MonEngine PUBLIC glm::glm)
target_link_libraries(MonEngine PUBLIC glad)

## LLGL
message("=== BUILDING LLGL ===")
add_subdirectory(${THIRD_PARTY_DIR}/LLGL)
set (LLGL_TARGETS
  LLGL
  LLGL_Direct3D11
  LLGL_Null
  LLGL_OpenGL
)
set_target_folder_prop(LLGL_TARGETS LLGL)
target_link_libraries(MonEngine PUBLIC LLGL)

## IMGUI
message("=== BUILDING IMGUI ===")
set(IMGUI_SRC 
  ${THIRD_PARTY_DIR}/imgui/imgui.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
  ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
  ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.cpp
)
add_library(imgui ${IMGUI_SRC})

set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")

target_include_directories(imgui PUBLIC
    ${THIRD_PARTY_DIR}/imgui
)

## TINYGLTF
message("=== BUILDING TINYGLTF ===")
add_subdirectory(${THIRD_PARTY_DIR}/tinygltf)
set(TINYGLTF_TARGETS
    tinygltf
    loader_example
)
set_target_folder_prop(TINYGLTF_TARGETS ThirdParty)

find_path(GAUSSIANLIB_INCLUDE_DIRS "Gauss/AffineMatrix.h")
target_include_directories(MonEngine PRIVATE ${GAUSSIANLIB_INCLUDE_DIRS})

## LUA
message("=== BUILDING LUA ===")
file(GLOB_RECURSE LUA_SOURCES "${THIRD_PARTY_DIR}/Lua/src/*.c")
source_group(TREE "${THIRD_PARTY_DIR}/Lua/src" FILES ${LUA_SOURCES})
add_library(lua STATIC ${LUA_SOURCES})
set_property(TARGET lua PROPERTY FOLDER "ThirdParty")
target_include_directories(lua PUBLIC
    ${THIRD_PARTY_DIR}/Lua/src
)
target_link_libraries(MonEngine PUBLIC lua)

## BULLET PHYSICS
message("=== BUILDING BULLET ===")
find_package(bullet CONFIG REQUIRED)

set (BULLET_LIBRARIES
  LinearMath
  Bullet3Common
  BulletInverseDynamics
  BulletCollision
  BulletDynamics
  BulletSoftBody
)

target_link_libraries(MonEngine PUBLIC ${BULLET_LIBRARIES})

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64

#  cmake -B ./Build -DGAME_TO_BUILD=dogeapp -DCMAKE_TOOLCHAIN_FILE=ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_MANIFEST_MODE=ON