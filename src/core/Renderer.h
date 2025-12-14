#pragma once
#include <unordered_map>
#include <string>

#include "Scene.h"
#include "graphics/Shader.h"
#include "graphics/buffers/VAO.h"
#include "graphics/buffers/VBO.h"
#include "../graphics/buffers/EBO.h"
#include "camera/CameraUBO.h"
#include <memory>


class Renderer {
public:
    void initialize();
    void render(Scene& scene, const InputHandler& inputHandler);
    static void reloadShaders() ;
    void setViewportSize(const int width, const int height) {
        screenWidth = width;
        screenHeight = height;
    }
    void resize(const int width, const int height) {
        screenWidth = width;
        screenHeight = height;
        glViewport(0, 0, width, height);
    }

private:
    // Resources
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
    CameraUBO m_cameraUBO;

    // Light Source Visualization
    VAO m_lightSourceVao;
    VBO m_lightVbo;
    EBO m_lightEbo;

    // Render Passes
    void renderOpaquePass(const Scene& scene, const InputHandler& inputHandler);
    void renderSkybox(const Scene& scene);
    void renderLightSource(); // Renders the white cube

    // Helpers
    void setupShaders();
    void setupLightCube();

    // Screen dimensions
    int screenWidth = 800;
    int screenHeight = 600;
};
