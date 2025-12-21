#pragma once
#include <vector>
#include <glad/glad.h>
#include "../graphics/Shader.h"
#include "../graphics/Texture.h"
#include "graphics/buffers/EBO.h"
#include "graphics/buffers/VAO.h"
#include "graphics/buffers/VBO.h"

struct TerrainParams {
    int octaves = 6;           // Number of noise layers
    float persistence = 0.5f;  // How much each octave contributes (0-1)
    float lacunarity = 2.0f;   // Frequency multiplier between octaves
    float noiseScale = 0.004;        // Overall terrain scale
    float heightMultiplier = 150.0f; // Overall height scaling
    float powerCurve = 4.0f;    // Exponent to shape terrain (higher = sharper peaks)
};

struct TerrainMaterial {
    float grassHeight = 2.0f;
    float rockHeight  = 40.0f;
    float snowHeight  = 120.0f;
};

class Terrain {
public:
    struct TerrainVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };
    Terrain(int worldWidth, int worldDepth, const std::vector<float>& heightMap);

    [[nodiscard]] std::vector<TerrainVertex> generateVertices() const;
    [[nodiscard]] std::vector<unsigned int> generateIndices() const;
    void calculateNormals(std::vector<TerrainVertex>& vertices, int worldWidth, int worldDepth) const;

    void loadTextures();

    void render(const Shader& shader) const;
    [[nodiscard]] glm::mat4 getModelMatrix() const;
    glm::vec3 m_position{0.0f};

private:
    void setupMesh(const std::vector<TerrainVertex>& vertices, const std::vector<unsigned int>& indices);

    std::vector<float> m_heights;

    VAO m_VAO;
    VBO m_VBO;
    EBO m_EBO;

    GLsizei m_indexCount;
    int m_worldWidth, m_worldDepth;

    Texture m_grassTexture;
    Texture m_grassNormal;
    Texture m_grassRoughness;
    Texture m_grassAO;

    Texture m_rockTexture;
    Texture m_rockNormal;
    Texture m_rockRoughness;
    Texture m_rockAO;

    Texture m_snowTexture;
    Texture m_snowNormal;
    Texture m_snowRoughness;
    Texture m_snowAO;
};




