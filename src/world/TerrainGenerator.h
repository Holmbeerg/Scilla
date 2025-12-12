#pragma once
#include <vector>

struct TerrainParams {
    // Base noise parameters
    int octaves = 6;           // Number of noise layers
    float persistence = 0.5f;  // How much each octave contributes (0-1)
    float lacunarity = 2.0f;   // Frequency multiplier between octaves
    float scale = 1.5f;        // Overall terrain scale
    float heightMultiplier = 150.0f;

    // Shaping parameters
    float powerCurve = 6;   // Higher = sharper peaks, flatter valleys
    float baseHeight = 0.0f;   // Minimum terrain height
};

class TerrainGenerator {
public:
    TerrainGenerator() = default;

    static std::vector<float> generateHeights(int width, int depth, const TerrainParams& params);

private:
    // Helper functions for different terrain features
    static float applyPowerCurve(float noise, float power);
};
