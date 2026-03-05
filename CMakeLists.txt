# CMakeLists.txt for 3D Engine project
cmake_minimum_required(VERSION 3.20)

# Project Name
project(ThreeDEngine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
)

# Application is of type WIN32 (GUI application)
add_executable(ThreeDEngine WIN32 ${ENGINE_SOURCES})

# Include directories
target_include_directories(ThreeDEngine PRIVATE
    includes
)

# Link libraries
target_link_directories(ThreeDEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)
target_link_libraries(ThreeDEngine PRIVATE
    opengl32
    user32
    gdi32
)
