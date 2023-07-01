# Generate Solution -> cmake -B ./Solution
# set cmake version
cmake_minimum_required(VERSION 3.18.2)

# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES "Source/*")
source_group(TREE ${PROJECT_SOURCE_DIR}/Game/Source PREFIX "Source Files/" FILES ${SOURCES})

add_executable(Game ${SOURCES})
set_target_properties(Game PROPERTIES LINKER_LANGUAGE CXX)

target_precompile_headers(
    Game 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <unordered_map>
    <functional>
)

message("CMAKE_CURRENT_SOURCE_DIR Game: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(
    Game
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
    Game 
    PRIVATE
    MonEngine
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64