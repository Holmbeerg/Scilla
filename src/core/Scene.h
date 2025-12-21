#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "camera/Camera.h"
#include "graphics/Model.h"
#include "world/Skybox.h"
#include "input/InputHandler.h"
#include "SceneObject.h"
#include "world/Terrain.h"
#include "world/VegetationPlacer.h"

class Scene {
public:
    Scene();

    // Prevent copying
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void initialize();
    void update(float deltaTime, const InputHandler& inputHandler);

    // Getters for the Renderer
    Camera& getCamera() { return m_camera; }
    [[nodiscard]] const Camera& getCamera() const { return m_camera; }

    [[nodiscard]] std::vector<SceneObject> getObjects() const { return m_objects; }

    [[nodiscard]] const Terrain& getTerrain() const { return *m_terrain; }
    void regenerateTerrain();

    [[nodiscard]] const Skybox& getSkybox() const { return *m_skybox; }
    [[nodiscard]] const VegetationPlacer& getVegetation() const { return *m_vegetation; }

    [[nodiscard]] glm::vec3 getSunDirection() const { return m_sunDirection; }
    [[nodiscard]] float getDayTime() const { return m_dayTime; }

    [[nodiscard]] const TerrainMaterial& getTerrainMaterial() const { return m_terrainMaterial; }
    [[nodiscard]] TerrainMaterial& getTerrainMaterialEdit() { return m_terrainMaterial; }

    void imGui();

private:
    Camera m_camera;
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<VegetationPlacer> m_vegetation;

    TerrainParams m_terrainParams;
    TerrainMaterial m_terrainMaterial;

    std::vector<SceneObject> m_objects;

    float m_dayTime = 0.0f;
    glm::vec3 m_sunDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};
