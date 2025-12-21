// Renderer is responsible for rendering the scene using OpenGL.
// It manages shaders, render passes, and drawing objects.

#include "Renderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "AssetManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
    m_shaders["vegetation"] = assetManager.loadShader(path + "vegetation.vert", path + "vegetation.frag");

    // Configure Light/Material Uniforms
    const auto objectShader = m_shaders["object"];
    objectShader->use();
    objectShader->setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
    objectShader->setVec3("light.ambient", glm::vec3(0.05f));
    objectShader->setVec3("light.diffuse", glm::vec3(0.5f));
    objectShader->setVec3("light.specular", glm::vec3(1.0f));
    objectShader->setFloat("material.shininess", 64.0f);

    const auto vegShader = m_shaders["vegetation"];
    vegShader->use();
    vegShader->setTextureUnit("material.diffuse", 0);
    vegShader->setTextureUnit("material.normal", 1);
    vegShader->setTextureUnit("material.arm", 2);
}

void Renderer::render(Scene &scene, const InputHandler &inputHandler) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start imGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw ImGui
    scene.imGui();

    // Update Camera UBO (Sends View/Proj/Pos to binding point 0)
    const Camera &cam = scene.getCamera();
    m_cameraUBO.setViewProjection(
        cam.getViewMatrix(),
        cam.getProjectionMatrix(static_cast<float>(screenWidth), static_cast<float>(screenHeight)),
        cam.getCameraPos());

    // Render Passes
    renderOpaquePass(scene, inputHandler);
    renderLightSource(); // Draw the light cube
    renderSkybox(scene); // Draw skybox last, reducing fragment shader calls

    // Finalize imGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::renderOpaquePass(const Scene &scene, const InputHandler &inputHandler) {
    const glm::vec3 sunDir = scene.getSunDirection();

    const auto terrainShader = m_shaders["terrain"];
    const auto& material = scene.getTerrainMaterial();
    terrainShader->use();
    terrainShader->setVec3("u_SunDirection", sunDir);
    terrainShader->setMat4("model", scene.getTerrain().getModelMatrix());
    terrainShader->setFloat("u_RockHeight", material.rockHeight);
    terrainShader->setFloat("u_SnowHeight", material.snowHeight);
    scene.getTerrain().render(*terrainShader);

    const auto vegShader = m_shaders["vegetation"];; // vegetation shader for trees, grass, etc.
    vegShader->use();
    vegShader->setVec3("u_SunDirection", sunDir); // sun moves so we set this uniform every frame

    const auto objShader = m_shaders["object"]; // object shader for things like trees, buildings, etc.
    objShader->use();
    objShader->setVec3("u_SunDirection", sunDir);
    objShader->setBool("enableNormalMapping", inputHandler.isNormalMappingEnabled());
    scene.getVegetation().render(*this, *vegShader);

    for (const auto &object : scene.getObjects()) {
        glm::mat4 modelMatrix = object.getTransform();

        // handle non-uniform scaling
        auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

        objShader->setMat4("model", modelMatrix);
        objShader->setMat3("normalMatrix", normalMatrix);

        object.model->render(*objShader);
    }
}

void Renderer::renderInstanced(const InstancedModel &batch, const Shader &shader) {
    if (batch.getInstanceCount() == 0) return;

    shader.use();

    for (const auto &mesh : batch.getModel()->getMeshes()) {
        mesh.getMaterial().bind(shader);

        const auto& vao = mesh.getVAO();
        glBindVertexArray(vao.getID());

        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(mesh.getIndexCount()),
            GL_UNSIGNED_INT,
            nullptr,
            batch.getInstanceCount()
        );
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
