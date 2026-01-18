# Example of how to use MonEngine as a submodule in your project
# This file shows different ways to integrate MonEngine

# Using add_subdirectory for the entire MonDev (includes Engine as submodule)
# Add this to your main CMakeLists.txt:
#
# add_subdirectory(path/to/MonDev)
# target_link_libraries(YourTarget PRIVATE MonEngine)

# need to include a copy of vcpkg.json in main project root
# Your project structure should look like:
# YourProject/
# ├── CMakeLists.txt
# ├── MonEngine/          # As git submodule
# │   ├── CMakeLists.txt
# │   ├── Source/
# │   └── ...
# └── src/
#     └── main.cpp
# |
# └── vcpkg.json <---- basically copy of file in MonDev but with a custom 'name'
