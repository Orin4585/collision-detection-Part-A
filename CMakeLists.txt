cmake_minimum_required(VERSION 3.10)
project(CircleCollision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Circle.cpp
)

# Create executable
add_executable(CircleCollision WIN32 ${SOURCES})

# Link with OpenGL libraries
target_link_libraries(CircleCollision
    opengl32
    glu32
)
