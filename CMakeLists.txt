# Generate Solution -> 'cmake -B ./Solution -A Win32'

# set cmake version
cmake_minimum_required(VERSION 3.18.2)

# set project name
project(MonDev)

set(SOURCE_PROJECT_DIR Source) 
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# initialize property for all targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CmakeGenerated")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(FOLDERS
    Source/Core
    Source/Entity
    Source/Entity/Components
    Source/Graphics
    Source/Input
    Source/Descriptions
    Source/Map
    Source/Map/Components
)

foreach(ITEM ${FOLDERS})
	file(GLOB FLS
		${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}/*.h)
	set(SOURCES ${SOURCES} ${FLS})	
	string(REPLACE / \\ SGROUP ${ITEM})
	source_group(${SGROUP} FILES ${FLS})
endforeach(ITEM)

add_executable(MonDev ${SOURCES})

target_include_directories(
    MonDev
    PUBLIC
    "Source/"
    "Source/Core"
    "Source/Entity"
    "Source/Entity/Components"
    "Source/Graphics"
    "Source/Input"
    "Source/Descriptions"
    "Source/Map"
    "Source/Map/Components"
)

# ENTT Library
add_subdirectory(${THIRD_PARTY_DIR}/entt)
target_include_directories(
    MonDev
    PUBLIC
    "${THIRD_PARTY_DIR}/entt/single_include"
)

# Json parsing library
add_subdirectory(${THIRD_PARTY_DIR}/json-develop)

# SDL + SDL Image 
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
target_link_libraries(
    MonDev 
    ${SDL2_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES}
    nlohmann_json::nlohmann_json
)

target_include_directories(
    MonDev 
    PUBLIC
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64