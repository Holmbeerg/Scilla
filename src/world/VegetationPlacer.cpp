#include "VegetationPlacer.h"
#include "../core/AssetManager.h"
#include "../core/Renderer.h"
#include <cstdlib>
#include <iostream>

void VegetationPlacer::generate(const Terrain &terrain, const std::vector<float> &heightMap, const int mapWidth) {
    auto &assets = AssetManager::get();
    srand(static_cast<unsigned>(time(nullptr)));

    m_small_tree = std::make_unique<InstancedModel>(
        assets.loadModel("/home/holmberg/development/Scilla/assets/models/tree/scene.gltf")
    );

    const glm::vec3 terrainPos = terrain.m_position;

    std::cout << "Terrain position: (" << terrainPos.x << ", " << terrainPos.y << ", " << terrainPos.z << ")\n";

    std::cout << "Max terrain height: << " << *std::ranges::max_element(heightMap) << "\n";
    std::cout << "Min terrain height: << " << *std::ranges::min_element(heightMap) << "\n";

    constexpr int treeCount = 200;
    int actuallyPlaced = 0;

    for (int i = 0; i < treeCount; i++) {
        float randomX = static_cast<float>(rand() % ((mapWidth - 1) * 100)) / 100.0f;
        float randomZ = static_cast<float>(rand() % ((mapWidth - 1) * 100)) / 100.0f;

        float worldX = randomX + terrainPos.x;
        float worldZ = randomZ + terrainPos.z;

        float worldY = getHeightAt(worldX, worldZ, terrain, heightMap, mapWidth);

        float sinkOffset = 3.0f; // hack to prevent floating trees for now
        glm::vec3 worldPos(worldX, worldY - sinkOffset, worldZ);

        if (worldPos.y > 12.0f || worldPos.y < 0.0f) continue;

        const float scale = 0.8f + (rand() % 50) / 100.0f;
        const auto rotY = static_cast<float>(rand() % 360);

        m_small_tree->addInstance(worldPos, glm::vec3(scale), rotY);
        actuallyPlaced++;
    }

    std::cout << "Placed " << actuallyPlaced << " trees\n";
    m_small_tree->finalize();
}

void VegetationPlacer::render(Renderer &renderer, const Shader &shader) const {
    if (m_small_tree) {
        renderer.renderInstanced(*m_small_tree, shader);
    }
    if (m_grass) {
        renderer.renderInstanced(*m_grass, shader);
    }
}

// Bilinear interpolation to get height at (x, z)
// This works by finding the four nearest heightmap points and interpolating between them
float VegetationPlacer::getHeightAt(const float x, const float z, const Terrain& terrain, const std::vector<float>& heightMap, const int mapWidth) {
    float localX = (x - terrain.m_position.x);
    float localZ = (z - terrain.m_position.z);

    if (localX < 0 || localX >= mapWidth - 1 || localZ < 0 || localZ >= mapWidth - 1) {
        return -100.0f;
    }

    int x0 = static_cast<int>(std::floor(localX));
    int z0 = static_cast<int>(std::floor(localZ));
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    float fracX = localX - x0;
    float fracZ = localZ - z0;

    float h00 = heightMap[z0 * mapWidth + x0];
    float h10 = heightMap[z0 * mapWidth + x1];
    float h01 = heightMap[z1 * mapWidth + x0];
    float h11 = heightMap[z1 * mapWidth + x1];

    float h0 = h00 * (1 - fracX) + h10 * fracX;
    float h1 = h01 * (1 - fracX) + h11 * fracX;
    float height = h0 * (1 - fracZ) + h1 * fracZ;

    return height + terrain.m_position.y;
}
