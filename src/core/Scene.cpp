#include "Scene.h"
#include <cmath> // For sin/cos

#include "world/Terrain.h"
#include "world/TerrainGenerator.h"

Scene::Scene() = default;

void Scene::initialize() {
    skybox = std::make_unique<Skybox>();

    TerrainParams params;
    params.octaves = 10;
    params.scale = 1.5f;
    params.persistence = 0.4f;
    params.lacunarity = 2.5f;
    params.heightMultiplier = 180.0f;
    params.powerCurve = 6.0f;
    params.baseHeight = -10.f;

    auto heightData = TerrainGenerator::generateHeights(1024, 1024, params);
    terrain = std::make_unique<Terrain>(1024, 1024, heightData);
}

void Scene::update(const float deltaTime, const InputHandler& inputHandler) {
    camera.processInput(inputHandler.getWindow(), deltaTime);

    m_dayTime += deltaTime * 0.01f;

    const float x = std::cos(m_dayTime);
    const float y = std::sin(m_dayTime);

    constexpr float tilt = glm::radians(45.0f);

    m_sunDirection.x = x;
    m_sunDirection.y = y * std::cos(tilt);
    m_sunDirection.z = y * std::sin(tilt) * -1.0f;

    m_sunDirection = glm::normalize(m_sunDirection);
}