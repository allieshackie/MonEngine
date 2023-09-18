# Generate Solution -> cmake -B ./Build
# cmake -B ./Build -DBUILD_GAME=ON

cmake_minimum_required(VERSION 3.18.2)

#  ============== PROJECT ================
project(MonDev)

option(BUILD_GAME "Build the game" ON)

# ============== CONFIG ================

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)

macro(ADD_DEFINE IDENT)
    add_definitions("-D${IDENT}")
endmacro()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeGenerated")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# ============== MACROS ==============
# TODO: Could add a check for BUILD_GAME that allows these macros to be defined in Game cmake
# else default to macros defined by engine for testing
add_compile_definitions(ENTITIES_FOLDER="D:/dev/MonDev/Game/Data/Entities/")
add_compile_definitions(FONTS_FOLDER="D:/dev/MonDev/Game/Data/Fonts/")
add_compile_definitions(LEVELS_FOLDER="D:/dev/MonDev/Game/Data/Levels/")
add_compile_definitions(MAPS_FOLDER="D:/dev/MonDev/Game/Data/Maps/")
add_compile_definitions(MODELS_FOLDER="D:/dev/MonDev/Game/Data/Models/")
add_compile_definitions(SHADERS_FOLDER="D:/dev/MonDev/Game/Data/Shaders/")
add_compile_definitions(TEXTURES_FOLDER="D:/dev/MonDev/Game/Data/Textures/")

# ============== ENGINE ==============
add_subdirectory(${PROJECT_SOURCE_DIR}/Engine)

# ============== GAME ================
if(${BUILD_GAME})
  add_compile_definitions(BUILD_GAME)
  add_subdirectory(${PROJECT_SOURCE_DIR}/Game)
endif()

# ============== THIRD PARTY ================
add_subdirectory(${THIRD_PARTY_DIR}/entt)
set_property(TARGET aob PROPERTY FOLDER "ThirdParty")

add_subdirectory(${THIRD_PARTY_DIR}/json)

add_subdirectory(${THIRD_PARTY_DIR}/glm)
set_property(TARGET uninstall PROPERTY FOLDER "ThirdParty")

add_subdirectory(${THIRD_PARTY_DIR}/LLGL)
set_target_properties(LLGL PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_Direct3D11 PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_Null PROPERTIES FOLDER LLGL)
set_target_properties(LLGL_OpenGL PROPERTIES FOLDER LLGL)

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

set(GLAD_SRC 
  ${THIRD_PARTY_DIR}/glad/src/glad.c
)
add_library(glad ${GLAD_SRC})

set_property(TARGET glad PROPERTY FOLDER "ThirdParty")

target_include_directories(glad PUBLIC 
  ${THIRD_PARTY_DIR}/glad/include
)

# SDL + SDL Image 
# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64