# Add the source files with proper folder structure
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
source_group(TREE ${PROJECT_SOURCE_DIR}/Sandbox/Source PREFIX "Source Files/" FILES ${SOURCES})

add_executable(Sandbox ${SOURCES})
copy_runtime_data(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/Data" sandbox)

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
