#include "TerrainGenerator.h"
#include <cmath>
#include <algorithm>
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>
#include <utils/Math.h>

// this generates a heightmap using fractal brownian motion (FBM) based on Perlin noise
std::vector<float> TerrainGenerator::generateHeights(const int worldWidth, const int worldDepth, const TerrainParams& params) {
    std::vector<float> heights(worldWidth * worldDepth);

    // Track the actual range of noise values we generate
    float minNoiseHeight = std::numeric_limits<float>::max();
    float maxNoiseHeight = std::numeric_limits<float>::lowest();

    // Pass 1 - Generate Raw Noise Heights
    for (int z = 0; z < worldDepth; z++) {
        for (int x = 0; x < worldWidth; x++) {

            float amplitude = 1.0f;
            float frequency = 1.0f;
            float noiseHeight = 0.0f;

            for (int i = 0; i < params.octaves; i++) {
                const float sampleX = (x * params.noiseScale) * frequency;
                const float sampleZ = (z * params.noiseScale) * frequency;

                // stb_perlin_noise3 returns [-1, 1] approximately
                const float perlinValue = stb_perlin_noise3(sampleX, 0.0f, sampleZ, 0, 0, 0);

                noiseHeight += perlinValue * amplitude;

                amplitude *= params.persistence;
                frequency *= params.lacunarity;
            }

            // Store raw value
            heights[z * worldWidth + x] = noiseHeight;

            // Track Min/Max
            if (noiseHeight > maxNoiseHeight) maxNoiseHeight = noiseHeight;
            if (noiseHeight < minNoiseHeight) minNoiseHeight = noiseHeight;
        }
    }

    // Pass 2 - Normalize & Apply Height Shaping
    for (int z = 0; z < worldDepth; z++) {
        for (int x = 0; x < worldWidth; x++) {
            const int index = z * worldWidth + x;
            // 1. Inverse Lerp: Map [min, max] to [0, 1]
            const float t = Math::inverseLerp(minNoiseHeight, maxNoiseHeight, heights[index]);

            // 2. Apply Power Curve (Shaping valleys/peaks)
            const float shapedHeight = applyPowerCurve(t, params.powerCurve);

            // 3. Final Scale
            heights[index] = shapedHeight * params.heightMultiplier;
        }
    }

    return heights;
}

float TerrainGenerator::applyPowerCurve(const float noise, const float power) {
    return std::pow(noise, power);
}