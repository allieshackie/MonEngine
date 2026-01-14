# cmake -B ./Build
# Building Sandbox: cmake -B ./Build -DPROJ_TO_BUILD=sandbox
# Building Game: cmake -B ./Build -DPROJ_TO_BUILD=game

cmake_minimum_required(VERSION 4.2.1)

#  ============== PROJECT ================
project(MonDev)

# Check if this is being used as a submodule
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(MONDEV_STANDALONE TRUE)
else()
    set(MONDEV_STANDALONE FALSE)
endif()

set(DCMAKE_TOOLCHAIN_FILE "ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake")

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
add_compile_definitions(ASSETS_FOLDER="${CMAKE_BINARY_DIR}/Data/Assets/")
add_compile_definitions(LEVELS_FOLDER="${CMAKE_BINARY_DIR}/Data/Levels/")
add_compile_definitions(PREFABS_FOLDER="${CMAKE_BINARY_DIR}/Data/Prefabs/")
add_compile_definitions(SHADERS_FOLDER="${CMAKE_BINARY_DIR}/Data/Shaders/")
add_compile_definitions(SCRIPTS_FOLDER="${CMAKE_BINARY_DIR}/Data/Scripts/")

# ============== ENGINE ==============
message("=== BUILDING ENGINE ===")
add_subdirectory(${PROJECT_SOURCE_DIR}/Engine)

# ====== BUILD SELECTED GAME =========
if(MONDEV_STANDALONE)
    if(PROJ_TO_BUILD STREQUAL "sandbox")
      message("=== BUILDING SANDBOX ===")
      add_subdirectory(${PROJECT_SOURCE_DIR}/Sandbox)
      set(PROJ_NAME "Sandbox")
    elseif(PROJ_TO_BUILD STREQUAL "game")
      message("=== BUILDING GAME ===")
      add_subdirectory(${PROJECT_SOURCE_DIR}/Game)
      set(PROJ_NAME "Game")
    else()
      message("Unknown Proj to build")
      set(PROJ_NAME "MonEngine")
    endif()

    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})

    # ================== DATA COPY ==================
    set(GAME_DATA_DIR "${PROJECT_SOURCE_DIR}/${PROJ_NAME}/Data")

    if (EXISTS ${GAME_DATA_DIR})
        file(GLOB_RECURSE GAME_DATA_FILES "${GAME_DATA_DIR}/*")

        add_custom_target(copy_data ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Data"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${GAME_DATA_DIR}"
                    "${CMAKE_BINARY_DIR}/Data"
            DEPENDS ${GAME_DATA_FILES}
        )

        add_dependencies(${PROJ_NAME} copy_data)
    else()
        message(WARNING "No Data folder found for ${PROJ_NAME}")
    endif()
else()
    message("MonDev used as submodule - Engine library available as 'MonEngine'")
endif()

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64
