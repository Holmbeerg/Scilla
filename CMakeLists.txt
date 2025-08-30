cmake_minimum_required(VERSION 3.10)
project(opengl_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(opengl_project
        src/main.cpp
        glad/glad.c
        src/graphics/Shader.h
        src/graphics/Shader.cpp
        stb/stb_image.cpp
        src/graphics/VAO.cpp
        src/graphics/VAO.h
        src/graphics/VBO.cpp
        src/graphics/VBO.h
        src/graphics/EBO.cpp
        src/graphics/EBO.h
        src/graphics/Texture.cpp
        src/graphics/Texture.h
        src/camera/Camera.cpp
        src/camera/Camera.h
        src/utils/FrameTimer.cpp
        src/utils/FrameTimer.h
        src/input/InputHandler.cpp
        src/input/InputHandler.h
        src/graphics/Model.cpp
        src/graphics/Model.h
        src/graphics/Mesh.cpp
        src/graphics/Mesh.h
)

target_include_directories(opengl_project PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/glad
)

target_link_libraries(opengl_project
        glfw
        OpenGL::GL
        dl)