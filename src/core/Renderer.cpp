// Renderer is responsible for rendering the scene using OpenGL.
// It manages shaders, render passes, and drawing objects.

#include "Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/Cube.h"

void Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    setupShaders();
    setupLightCube();
}

void Renderer::setupShaders() {
    const std::string path = "assets/shaders/";
    m_shaders["object"] = Shader(path + "vertex_shader.vert", path + "fragment_shader.frag");
    m_shaders["light"] = Shader(path + "vertex_shader.vert", path + "lightSource.frag");
    m_shaders["skybox"] = Shader(path + "skybox.vert", path + "skybox.frag");

    // Configure Skybox Texture Unit
    m_shaders["skybox"].use();
    m_shaders["skybox"].setInt("skybox", 0);

    // Configure Light/Material Uniforms
    m_shaders["object"].use();
    m_shaders["object"].setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
    m_shaders["object"].setVec3("light.ambient", glm::vec3(0.05f));
    m_shaders["object"].setVec3("light.diffuse", glm::vec3(0.5f));
    m_shaders["object"].setVec3("light.specular", glm::vec3(1.0f));
    m_shaders["object"].setFloat("material.shininess", 64.0f);
}

void Renderer::render(Scene &scene, const InputHandler &inputHandler) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Update Camera UBO (Sends View/Proj/Pos to binding point 0)
    const Camera &cam = scene.getCamera();

    m_cameraUBO.setViewProjection(
        cam.getViewMatrix(),
        cam.getProjectionMatrix(static_cast<float>(screenWidth), static_cast<float>(screenHeight)),
        cam.getCameraPos());

    // 2. Render Passes
    renderOpaquePass(scene, inputHandler);
    renderLightSource(); // Draw the light cube
    renderSkybox(scene); // Draw skybox last, reducing fragment shader calls
}

void Renderer::renderOpaquePass(const Scene &scene, const InputHandler &inputHandler) {
    const Shader &shader = m_shaders["object"];
    shader.use();

    shader.setBool("enableNormalMapping", inputHandler.isNormalMappingEnabled());

    for (const auto &model: scene.getModels()) {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -3.0f));

        auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

        shader.setMat4("model", modelMatrix);
        shader.setMat3("normalMatrix", normalMatrix);

        model->render(shader);
    }
}

void Renderer::renderLightSource() {
    const Shader &shader = m_shaders["light"];
    shader.use();

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);

    m_lightSourceVao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::setupLightCube() {
    m_lightSourceVao.generate();
    m_lightVbo.generate();
    m_lightEbo.generate();

    m_lightSourceVao.bindVBO(m_lightVbo, 0, 8 * sizeof(float)); // Stride 8 floats (3 pos, 3 normal, 2 texcoords)
    m_lightVbo.setData(Primitives::cubeVertices, sizeof(Primitives::cubeVertices));

    m_lightSourceVao.enableAttrib(0);
    m_lightSourceVao.setAttribFormat(0, 3, GL_FLOAT, false, 0, 0); // Pos
    m_lightSourceVao.bind();
}

void Renderer::renderSkybox(const Scene &scene) {
    glDepthFunc(GL_LEQUAL); // Optimization: Only draw sky where depth is <= 1.0

    const Shader &shader = m_shaders["skybox"];
    shader.use();

    scene.getSkybox().render(shader);

    glDepthFunc(GL_LESS); // Reset
}

void Renderer::reloadShaders() {
    std::cout << "Reloading shaders..." << std::endl;
    for (auto &pair: m_shaders) {
        pair.second.reload();
    }
    // Re-set uniforms after reload
    m_shaders["skybox"].use();
    m_shaders["skybox"].setInt("skybox", 0);
    // Re-set light/material properties?
}
