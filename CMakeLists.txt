cmake_minimum_required(VERSION 3.10)
project(Scilla)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(assimp REQUIRED)

add_library(third_party STATIC
        glad/glad.c
        stb/stb_image.cpp
)

target_include_directories(third_party PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/stb
)

add_executable(scilla
        src/main.cpp
        src/graphics/Shader.h
        src/graphics/Shader.cpp
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
        src/graphics/Cube.h
        src/graphics/Skybox.cpp
        src/graphics/Skybox.h
)

target_include_directories(scilla PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(scilla
        third_party
        glfw
        OpenGL::GL
        dl
        assimp::assimp)