#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "camera/Camera.h"
#include "graphics/Model.h"
#include "world/Skybox.h"
#include "input/InputHandler.h"
#include "world/Terrain.h"

class Scene {
public:
    Scene();

    // Prevent copying
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void initialize();
    void update(float deltaTime, const InputHandler& inputHandler);

    // Getters for the Renderer
    Camera& getCamera() { return camera; }
    [[nodiscard]] const Camera& getCamera() const { return camera; }

    [[nodiscard]] const std::vector<std::unique_ptr<Model>>& getModels() const { return models; }
    [[nodiscard]] const Terrain& getTerrain() const { return *terrain; }
    [[nodiscard]] const Skybox& getSkybox() const { return *skybox; }

    [[nodiscard]] glm::vec3 getSunDirection() const { return m_sunDirection; }
    [[nodiscard]] float getDayTime() const { return m_dayTime; }

private:
    Camera camera;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Terrain> terrain;
    std::vector<std::unique_ptr<Model>> models;

    float m_dayTime = 0.0f;
    glm::vec3 m_sunDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};