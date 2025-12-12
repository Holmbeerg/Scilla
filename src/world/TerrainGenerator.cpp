#include "TerrainGenerator.h"
#include <cmath>
#include <algorithm>
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

std::vector<float> TerrainGenerator::generateHeights(const int width, const int depth, const TerrainParams& params) {
    std::vector<float> heights(width * depth);

    // First pass: Generate base noise (can add more passes like erosion later)
    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;
            float maxValue = 0.0f; // For normalization

            // FBM: Stack multiple noise octaves
            for (int i = 0; i < params.octaves; i++) {
                const float sampleX = (x * params.scale * frequency) / 100.0f;
                const float sampleZ = (z * params.scale * frequency) / 100.0f;

                // Perlin noise returns values in [-1, 1]
                const float perlinValue = stb_perlin_noise3(sampleX, 0.0f, sampleZ, 0, 0, 0);

                noiseHeight += perlinValue * amplitude;
                maxValue += amplitude;

                amplitude *= params.persistence;
                frequency *= params.lacunarity;
            }

            // Normalize to [0, 1] range
            const float normalized = (noiseHeight / maxValue + 1.0f) / 2.0f;

            // Apply power curve for mountain shaping
            const float shaped = applyPowerCurve(normalized, params.powerCurve);

            // Scale to final height
            const float finalHeight = params.baseHeight + (shaped * params.heightMultiplier);

            heights[z * width + x] = finalHeight;
        }
    }
    return heights;
}

float TerrainGenerator::applyPowerCurve(const float noise, const float power) {
    // Higher power = sharper peaks and flatter valleys
    return std::pow(noise, power);
}