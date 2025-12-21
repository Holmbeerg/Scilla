#pragma once
#include <vector>
#include <memory>
#include "../graphics/InstancedModel.h"
#include "Terrain.h"

// Forward declaration. do like this or include the headers?
class Renderer;
class Shader;

class VegetationPlacer {
public:
    VegetationPlacer() = default;

    // Call this once during Scene::initialize
    void generate(const Terrain& terrain, const std::vector<float>& heightMap, int mapWidth);
    void render(Renderer& renderer, const Shader& shader) const;

private:
    // The Batches
    std::unique_ptr<InstancedModel> m_small_tree;
    std::unique_ptr<InstancedModel> m_grass;
};