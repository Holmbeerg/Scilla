// Scene represents the 3D world and its state, containing models, skybox, and camera.

#include "Scene.h"

Scene::Scene() = default;

void Scene::initialize() {
    models.push_back(std::make_unique<Model>("assets/models/backpack/backpack.obj"));

    const std::vector<std::string> faces = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };
    skybox = std::make_unique<Skybox>(faces);
}

void Scene::update(const float deltaTime, const InputHandler& inputHandler) {
    camera.processInput(inputHandler.getWindow(), deltaTime);
}