# Generate Solution -> cmake -B ./Solution
# set cmake version
cmake_minimum_required(VERSION 3.18.2)

# Include the function
include(${CMAKE_UTILS}/CmakeUtils.cmake)

# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES "Source/*")
source_group(TREE ${PROJECT_SOURCE_DIR}/Sandbox/Source PREFIX "Source Files/" FILES ${SOURCES})

add_executable(Sandbox ${SOURCES})
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)

# Call the function to copy the Data folder contents to the Build folder
copy_data_contents(${CMAKE_CURRENT_SOURCE_DIR}/Data/ ${CMAKE_BINARY_DIR}/Data)

target_precompile_headers(
    Sandbox 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <istream>
    <unordered_map>
    <functional>
)

message("CMAKE_CURRENT_SOURCE_DIR Sandbox: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(
    Sandbox
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
    Sandbox 
    PRIVATE
    Engine
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64