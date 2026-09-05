# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
source_group(TREE ${PROJECT_SOURCE_DIR}/Game/Source PREFIX "Source Files/" FILES ${SOURCES})

add_executable(Game ${SOURCES})
copy_runtime_data(Game "${CMAKE_CURRENT_SOURCE_DIR}/Data" game)

target_precompile_headers(
    Game 
    PRIVATE
    <iostream>
    <memory>
    <vector>
    <fstream>
    <istream>
    <unordered_map>
    <functional>
)

target_include_directories(
    Game
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
    Game 
    PRIVATE
    Engine
)

# target_include_directories = Include Directories
# target_link_libraries = Additional Dependencies (lib)
# link_directories = Additional Library Directories (dll) x64
