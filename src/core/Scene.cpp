// The Scene owns the instances of all objects in the world.
// Scene does not load models/textures itself; that is handled by AssetManager.

#include "Scene.h"
#include <cmath> // For sin/cos
#include <imgui.h>

#include "AssetManager.h"
#include "world/Terrain.h"
#include "world/TerrainGenerator.h"

Scene::Scene() = default;

void Scene::initialize() {
    auto& assets = AssetManager::get();
    m_skybox = std::make_unique<Skybox>();

    auto heightData = TerrainGenerator::generateHeights(2048, 2048, m_terrainParams);
    m_terrain = std::make_unique<Terrain>(2048, 2048, heightData);

    m_vegetation = std::make_unique<VegetationPlacer>();
    m_vegetation->generate(*m_terrain, heightData, 2048);

    m_camera.setPosition(1024.0f, 300.0f, 0.0f);
}

void Scene::update(const float deltaTime, const InputHandler& inputHandler) {
    m_camera.processInput(inputHandler.getWindow(), deltaTime);

    m_dayTime += deltaTime * 0.01f;

    const float x = std::cos(m_dayTime);
    const float y = std::sin(m_dayTime);

    constexpr float tilt = glm::radians(45.0f);

    m_sunDirection.x = x;
    m_sunDirection.y = y * std::cos(tilt);
    m_sunDirection.z = y * std::sin(tilt) * -1.0f;

    m_sunDirection = glm::normalize(m_sunDirection);
}

void Scene::regenerateTerrain() {
    auto heightData = TerrainGenerator::generateHeights(2048, 2048, m_terrainParams);
    m_terrain = std::make_unique<Terrain>(2048, 2048, heightData);

    m_vegetation->generate(*m_terrain, heightData, 2048);
}

void Scene::imGui() {
    ImGui::Begin("Terrain Tools");

    // Terrain Generation Controls
    if (ImGui::TreeNode("Shape Generation")) {
        ImGui::SliderFloat("Scale", &m_terrainParams.noiseScale, 0.001f, 0.1f);
        ImGui::SliderFloat("Height", &m_terrainParams.heightMultiplier, 10.0f, 500.0f);
        ImGui::SliderInt("Octaves", &m_terrainParams.octaves, 1, 10);
        ImGui::SliderFloat("Power", &m_terrainParams.powerCurve, 1.0f, 10.0f);

        if (ImGui::Button("Regenerate Terrain")) {
            regenerateTerrain();
        }
        ImGui::TreePop();
    }

    // Material & Shader Controls
    if (ImGui::TreeNode("Material & Shader")) {
        ImGui::SliderFloat("Grass Limit", &m_terrainMaterial.grassHeight, 0.0f, 50.0f);
        ImGui::SliderFloat("Rock Limit", &m_terrainMaterial.rockHeight, 0.0f, 100.0f);
        ImGui::SliderFloat("Snow Limit", &m_terrainMaterial.snowHeight, 50.0f, 200.0f);
        ImGui::TreePop();
    }

    ImGui::End();
}