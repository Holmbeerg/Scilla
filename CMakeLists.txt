cmake_minimum_required(VERSION 3.30)
project(opengl_project)
find_package(glfw3 3.4 REQUIRED)
add_executable(opengl_project
        main.cpp
        glad/glad.c)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_link_libraries(opengl_project
        glfw
        GL
)
set(CMAKE_CXX_STANDARD 26)
