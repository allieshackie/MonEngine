# Generate Solution -> cmake -B ./Build
# cmake -B ./Build -DMON_DEV_BUILD_SANDBOX=ON

cmake_minimum_required(VERSION 3.18.2)

#  ============== PROJECT ================
project(MonDev)

option(MON_DEV_BUILD_SANDBOX "Build the sandbox game" ON)
option(MON_DEV_BUILD_GAME "Build the game" ON)

# ============== CONFIG ================

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)
set(CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

# Macros must be defined before subdirectories are added
# ============== MACROS ===============
add_compile_definitions(ENTITIES_FOLDER="${CMAKE_BINARY_DIR}/Data/Entities/")
add_compile_definitions(FONTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Fonts/")
add_compile_definitions(LEVELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Levels/")
add_compile_definitions(MAPS_FOLDER="${CMAKE_BINARY_DIR}/Data/Maps/")
add_compile_definitions(MODELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Models/")
add_compile_definitions(SHADERS_FOLDER="${CMAKE_BINARY_DIR}/Data/Shaders/")
add_compile_definitions(TEXTURES_FOLDER="${CMAKE_BINARY_DIR}/Data/Textures/")
add_compile_definitions(UI_FOLDER="${CMAKE_BINARY_DIR}/Data/UI/")

if(${MON_DEV_BUILD_SANDBOX} OR ${MON_DEV_BUILD_GAME})
  add_compile_definitions(BUILD_GAME)
endif()

# ============== ENGINE ==============
add_subdirectory(${PROJECT_SOURCE_DIR}/Engine)

# ============== GAME ================
if(${MON_DEV_BUILD_SANDBOX})
  add_subdirectory(${PROJECT_SOURCE_DIR}/Game)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Entities) 
  message("MONDEV ERROR: Data/Entities folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Fonts)
  message("MONDEV ERROR: Data/Fonts folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Levels)
  message("MONDEV ERROR: Data/Levels folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Maps)
  message("MONDEV ERROR: Data/Maps folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Models)
  message("MONDEV ERROR: Data/Models folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Shaders)
  message("MONDEV ERROR: Data/Shaders folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/Textures)
  message("MONDEV ERROR: Data/Textures folder isn't located in /Build")
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/Data/UI)
  message("MONDEV ERROR: Data/UI folder isn't located in /Build")
endif()

# ============== THIRD PARTY ================
add_subdirectory(${THIRD_PARTY_DIR}/entt)
#set_property(TARGET aob PROPERTY FOLDER "ThirdParty")

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

# SDL + SDL Image 
# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64