# Generate Solution -> cmake -B ./Build
# cmake -B ./Build -DMON_DEV_BUILD_SANDBOX=ON

cmake_minimum_required(VERSION 3.18.2)

#  ============== PROJECT ================
project(MonDev)

option(MON_DEV_BUILD_SANDBOX "Build the sandbox game" OFF)
option(MON_DEV_BUILD_GAME "Build the game" OFF)

# ============== CONFIG ================

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)
set(CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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


if(${MON_DEV_BUILD_SANDBOX} OR ${MON_DEV_BUILD_GAME})
  add_compile_definitions(BUILD_GAME)
  # Macros must be defined before subdirectories are added
  # ============== MACROS ===============
  add_compile_definitions(ENTITIES_FOLDER="${CMAKE_BINARY_DIR}/Data/Entities/")
  add_compile_definitions(FONTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Fonts/")
  add_compile_definitions(LEVELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Levels/")
  add_compile_definitions(MAPS_FOLDER="${CMAKE_BINARY_DIR}/Data/Maps/")
  add_compile_definitions(MODELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Models/")
  add_compile_definitions(SHADERS_FOLDER="${CMAKE_BINARY_DIR}/Data/Shaders/")
  add_compile_definitions(SCRIPTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Scripts/")
  add_compile_definitions(LUA_MODULES_FOLDER="${CMAKE_BINARY_DIR}/Data/LuaModules/")
  add_compile_definitions(TEXTURES_FOLDER="${CMAKE_BINARY_DIR}/Data/Textures/")
endif()

# ============== ENGINE ==============
add_subdirectory(${PROJECT_SOURCE_DIR}/Engine)

# ============== GAME ================
if(${MON_DEV_BUILD_SANDBOX})
  add_subdirectory(${PROJECT_SOURCE_DIR}/Game)
endif()

# ============== THIRD PARTY ================

## ENTT
add_subdirectory(${THIRD_PARTY_DIR}/entt)
#set_property(TARGET aob PROPERTY FOLDER "ThirdParty")

## JSON
add_subdirectory(${THIRD_PARTY_DIR}/json)

## GLM
add_subdirectory(${THIRD_PARTY_DIR}/glm)
set_property(TARGET uninstall PROPERTY FOLDER "ThirdParty")

## LLGL
add_subdirectory(${THIRD_PARTY_DIR}/LLGL)
set_target_properties(LLGL PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_Direct3D11 PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_Null PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_OpenGL PROPERTIES FOLDER LLGL)

## IMGUI
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
add_subdirectory(${THIRD_PARTY_DIR}/tinygltf)

set_property(TARGET tinygltf PROPERTY FOLDER "ThirdParty")
set_property(TARGET loader_example PROPERTY FOLDER "ThirdParty")

## LUA
file(GLOB_RECURSE LUA_SOURCES "${THIRD_PARTY_DIR}/Lua/src/*.c")
source_group(TREE "${THIRD_PARTY_DIR}/Lua/src" FILES ${LUA_SOURCES})

add_library(lua STATIC ${LUA_SOURCES})

set_property(TARGET lua PROPERTY FOLDER "ThirdParty")

target_include_directories(lua PUBLIC
    ${THIRD_PARTY_DIR}/Lua/src
)

## BULLET PHYSICS
add_subdirectory(${THIRD_PARTY_DIR}/bullet3)
set_property(TARGET Bullet2FileLoader PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET Bullet3Collision PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET Bullet3Common PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET Bullet3Dynamics PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET Bullet3Geometry PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET Bullet3OpenCL_clew PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET BulletCollision PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET BulletDynamics PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET BulletInverseDynamics PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET BulletSoftBody PROPERTY FOLDER "ThirdParty/bullet3")
set_property(TARGET LinearMath PROPERTY FOLDER "ThirdParty/bullet3")

set (BULLET_LIBRARIES    "LinearMath;Bullet3Common;BulletInverseDynamics;BulletCollision;BulletDynamics;BulletSoftBody")
set (BULLET_INCLUDE_DIRS ${THIRD_PARTY_DIR}/bullet3/src)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64