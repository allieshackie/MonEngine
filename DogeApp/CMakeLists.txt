# Generate Solution -> cmake -B ./Solution
# set cmake version
cmake_minimum_required(VERSION 3.18.2)

# Include the function
include(${CMAKE_UTILS}/CmakeUtils.cmake)

# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES "Source/*")
source_group(TREE ${PROJECT_SOURCE_DIR}/DogeApp/Source PREFIX "Source Files/" FILES ${SOURCES})

add_executable(DogeApp ${SOURCES})
set_target_properties(DogeApp PROPERTIES LINKER_LANGUAGE CXX)

# Call the function to copy the Data folder contents to the Build folder
copy_data_contents(${CMAKE_CURRENT_SOURCE_DIR}/Data/ ${CMAKE_BINARY_DIR}/Data)

target_precompile_headers(
    DogeApp 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <istream>
    <unordered_map>
    <functional>
)

message("CMAKE_CURRENT_SOURCE_DIR DogeApp: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(
    DogeApp
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
    DogeApp 
    PRIVATE
    MonEngine
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64