#pragma once
#include <vector>

#include "Terrain.h"

class TerrainGenerator {
public:
    TerrainGenerator() = default;

    static std::vector<float> generateHeights(int worldWidth, int worldDepth, const TerrainParams& params);

private:
    // Helper functions for different terrain features
    static float applyPowerCurve(float noise, float power);
};
