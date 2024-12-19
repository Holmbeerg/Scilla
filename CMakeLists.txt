cmake_minimum_required(VERSION 3.10)
project(opengl_project)

# Explicitly set the include directories
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/glad
        ${OPENGL_INCLUDE_DIRS}
)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(opengl_project
        src/main.cpp
        glad/glad.c
        src/Shader.h
        src/Shader.cpp
        stb/stb_image.cpp
        src/VAO.cpp
        src/VAO.h
        src/VBO.cpp
        src/VBO.h
        src/EBO.cpp
        src/EBO.h
        src/Texture.cpp
        src/Texture.h
        src/Camera.cpp
        src/Camera.h
        src/FrameTimer.cpp
        src/FrameTimer.h
        src/InputHandler.cpp
        src/InputHandler.h
)

target_include_directories(opengl_project PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/glad
)

target_link_libraries(opengl_project
        glfw
        OpenGL::GL
        dl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)