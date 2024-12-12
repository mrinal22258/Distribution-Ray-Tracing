cmake_minimum_required(VERSION 3.10)

project(Distribution_Ray_Tracing)

set(CMAKE_CXX_STANDARD 17)

# Find required libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB SRC_FILES src/*.cpp src/*.c)
file(GLOB GLAD_FILES src/glad.c)

# Add executable
add_executable(ray_tracer ${SRC_FILES} ${GLAD_FILES})

# Link libraries
target_link_libraries(ray_tracer OpenGL::GL glfw)
