// Renderer is responsible for rendering the scene using OpenGL.
// It manages shaders, render passes, and drawing objects.

#include "Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "AssetManager.h"
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
    auto& assetManager = AssetManager::get();
    m_shaders["object"] = assetManager.loadShader(path + "vertex_shader.vert", path + "fragment_shader.frag");
    m_shaders["light"]  = assetManager.loadShader(path + "vertex_shader.vert", path + "lightSource.frag");
    m_shaders["skybox"] = assetManager.loadShader(path + "skybox.vert", path + "procedural_sky.frag");
    m_shaders["terrain"] = assetManager.loadShader(path + "terrain.vert", path + "terrain.frag");

    // Configure Light/Material Uniforms
    const auto objectShader = m_shaders["object"];
    objectShader->use();
    objectShader->setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
    objectShader->setVec3("light.ambient", glm::vec3(0.05f));
    objectShader->setVec3("light.diffuse", glm::vec3(0.5f));
    objectShader->setVec3("light.specular", glm::vec3(1.0f));
    objectShader->setFloat("material.shininess", 64.0f);
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
    const glm::vec3 sunDir = scene.getSunDirection();

    const auto terrainShader = m_shaders["terrain"];
    terrainShader->use();
    terrainShader->setVec3("u_SunDirection", sunDir);
    terrainShader->setMat4("model", scene.getTerrain().getModelMatrix());

    scene.getTerrain().render(*terrainShader);

    const auto objShader = m_shaders["object"];
    objShader->use();
    objShader->setVec3("u_SunDirection", sunDir);
    objShader->setBool("enableNormalMapping", inputHandler.isNormalMappingEnabled());

    for (const auto &object : scene.getObjects()) {
        glm::mat4 modelMatrix = object.getTransform();

        // handle non-uniform scaling
        auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

        objShader->setMat4("model", modelMatrix);
        objShader->setMat3("normalMatrix", normalMatrix);

        object.model->render(*objShader);
    }
}

void Renderer::renderLightSource() {
    const auto shader = m_shaders["light"];
    shader->use();

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader->setMat4("model", model);

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
    glDepthFunc(GL_LEQUAL);

    const auto shader = m_shaders["skybox"];
    shader->use();

    scene.getSkybox().render(
        *shader,
        scene.getSunDirection(),
        scene.getDayTime()
    );

    glDepthFunc(GL_LESS);
}

void Renderer::reloadShaders() {
    std::cout << "Reloading shaders..." << std::endl;
    AssetManager::get().reloadShaders();
}
