file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${SOURCES})

add_executable(Tests ${SOURCES})
copy_runtime_data(Tests "${CMAKE_CURRENT_SOURCE_DIR}/Data" tests)

target_compile_features(Tests PRIVATE cxx_std_17)
set_target_properties(Tests PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

find_package(Catch2 3 CONFIG REQUIRED)
target_link_libraries(Tests PRIVATE MonDev::Engine Catch2::Catch2WithMain)

include("${Catch2_DIR}/Catch.cmake")
catch_discover_tests(Tests)
