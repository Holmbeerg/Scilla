cmake_minimum_required(VERSION 3.10)
project(Scilla)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(assimp REQUIRED)
find_package(X11 REQUIRED)

add_library(third_party STATIC
        glad/glad.c
        stb/stb_image.cpp

        # ImGui core files
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp

        # ImGui backends
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
)

target_include_directories(third_party PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/stb
        ${CMAKE_SOURCE_DIR}/imgui
)

add_executable(scilla
        src/main.cpp
        src/graphics/Shader.h
        src/graphics/Shader.cpp
        src/graphics/buffers/VAO.cpp
        src/graphics/buffers/VAO.h
        src/graphics/buffers/VBO.cpp
        src/graphics/buffers/VBO.h
        src/graphics/buffers/EBO.cpp
        src/graphics/buffers/EBO.h
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
        src/world/Skybox.cpp
        src/world/Skybox.h
        src/core/Engine.cpp
        src/core/Engine.h
        src/core/Scene.cpp
        src/core/Scene.h
        src/core/Renderer.cpp
        src/core/Renderer.h
        src/world/Terrain.cpp
        src/world/Terrain.h
        src/world/TerrainGenerator.h
        src/graphics/buffers/UniformBuffer.h
        src/graphics/buffers/UniformBuffer.cpp
        src/camera/CameraUBO.cpp
        src/camera/CameraUBO.h
        src/world/TerrainGenerator.cpp
        src/core/AssetManager.cpp
        src/core/AssetManager.h
        src/graphics/Material.h
        src/core/SceneObject.h
        src/world/VegetationPlacer.cpp
        src/world/VegetationPlacer.h
        src/graphics/buffers/InstanceBuffer.cpp
        src/graphics/buffers/InstanceBuffer.h
        src/graphics/InstancedModel.cpp
        src/graphics/InstancedModel.h
        src/utils/Math.h
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
        assimp::assimp
        X11::X11
)