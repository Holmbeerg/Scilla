// Scene represents the 3D world and its state, containing models, skybox, and camera.

#include "Scene.h"

Scene::Scene() = default;

void Scene::initialize() {
    models.push_back(std::make_unique<Model>("assets/models/backpack/backpack.obj"));
    skybox = std::make_unique<Skybox>();
}

void Scene::update(const float deltaTime, const InputHandler& inputHandler) {
    camera.processInput(inputHandler.getWindow(), deltaTime);

    if (skybox) {
        skybox->update(deltaTime);
    }
}