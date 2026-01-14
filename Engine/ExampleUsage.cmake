# Example of how to use MonEngine as a submodule in your project
# This file shows different ways to integrate MonEngine

# Method 1: Using add_subdirectory for the entire MonDev (includes Engine as submodule)
# Add this to your main CMakeLists.txt:
#
# add_subdirectory(path/to/MonDev)
# target_link_libraries(YourTarget PRIVATE MonEngine)
#
# This includes the entire MonDev as a submodule but only builds the Engine library

# Method 2: Using find_package (after installing MonEngine)
# First install MonEngine:
# cmake -B build -S path/to/MonEngine
# cmake --build build --target install
#
# Then in your CMakeLists.txt:
# find_package(MonEngine REQUIRED)
# target_link_libraries(YourTarget PRIVATE MonEngine::MonEngine)

# Method 3: Manual inclusion (not recommended)
# target_include_directories(YourTarget PRIVATE path/to/MonEngine/Source)
# target_link_libraries(YourTarget PRIVATE
#     # List all MonEngine dependencies manually...
# )

# Required dependencies for your project:
# - CMake 3.18.2 or higher
# - vcpkg with the following packages:
#   - entt
#   - nlohmann-json
#   - glm
#   - lua
#   - bullet3
#
# Your project structure should look like:
# YourProject/
# ├── CMakeLists.txt
# ├── MonEngine/          # As git submodule
# │   ├── CMakeLists.txt
# │   ├── Source/
# │   └── ...
# ├── ThirdParty/         # As git submodule or copied
# │   ├── imgui/
# │   ├── LLGL/
# │   ├── tinygltf/
# │   └── vcpkg/
# └── src/
#     └── main.cpp
