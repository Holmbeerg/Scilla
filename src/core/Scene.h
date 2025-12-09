#pragma once
#include <vector>
#include "camera/Camera.h"
#include "graphics/Model.h"
#include "world/Skybox.h"
#include "input/InputHandler.h"

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
    [[nodiscard]] const Skybox& getSkybox() const { return *skybox; }

private:
    Camera camera;
    std::unique_ptr<Skybox> skybox;
    std::vector<std::unique_ptr<Model>> models;
};