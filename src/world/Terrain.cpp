#include "Terrain.h"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>

Terrain::Terrain(const int worldWidth, const int worldDepth, const std::vector<float>& heightMap)
    : m_heights(heightMap),
      m_indexCount(0),
      m_worldWidth(worldWidth),
      m_worldDepth(worldDepth) {

    std::vector<TerrainVertex> vertices = generateVertices();
    calculateNormals(vertices, m_worldWidth, m_worldDepth);
    const std::vector<unsigned int> indices = generateIndices();

    m_indexCount = static_cast<GLsizei>(indices.size());
    setupMesh(vertices, indices);
    loadTextures();

    std::cout << indices.size() / 3 << " total triangles in terrain mesh." << std::endl;
}

std::vector<Terrain::TerrainVertex> Terrain::generateVertices() const {
    std::vector<TerrainVertex> vertices;
    vertices.reserve(m_worldWidth * m_worldDepth);

    for (int z = 0; z < m_worldDepth; z++) {
        for (int x = 0; x < m_worldWidth; x++) {
            const float y = m_heights[z * m_worldWidth + x];

            TerrainVertex v{};
            v.Position = glm::vec3(x, y, z);
            v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            v.TexCoords = glm::vec2(
                static_cast<float>(x) / m_worldWidth,
                static_cast<float>(z) / m_worldDepth
            );

            vertices.push_back(v);
        }
    }

    return vertices;
}

std::vector<unsigned int> Terrain::generateIndices() const {
    std::vector<unsigned int> indices;
    indices.reserve((m_worldWidth - 1) * (m_worldDepth - 1) * 6);

    for (int z = 0; z < m_worldDepth - 1; z++) {
        for (int x = 0; x < m_worldWidth - 1; x++) {
            const int topLeft = (z * m_worldWidth) + x;
            const int topRight = topLeft + 1;
            const int bottomLeft = ((z + 1) * m_worldWidth) + x;
            const int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return indices;
}

void Terrain::calculateNormals(std::vector<TerrainVertex>& vertices, const int worldWidth, const int worldDepth) const {
    auto getHeightSafe = [&](int x, int z) -> float {
        x = std::max(0, std::min(x, worldWidth - 1));
        z = std::max(0, std::min(z, worldDepth - 1));
        return m_heights[z * worldWidth + x];
    };

    for (int z = 1; z < m_worldDepth - 1; z++) {
        for (int x = 1; x < m_worldWidth - 1; x++) {
            int idx = z * m_worldWidth + x;

            float hL = getHeightSafe(x - 1, z);
            float hR = getHeightSafe(x + 1, z);
            float hD = getHeightSafe(x, z - 1);
            float hU = getHeightSafe(x, z + 1);

            // Calculate normal using finite difference
            glm::vec3 normal;
            normal.x = hL - hR;
            normal.y = 2.0f;
            normal.z = hD - hU;
            vertices[idx].Normal = glm::normalize(normal);

            // Calculate tangent and bitangent for normal mapping
            glm::vec3 tangentX(2.0f, hR - hL, 0.0f);
            vertices[idx].Tangent = glm::normalize(tangentX);

            glm::vec3 tangentZ(0.0f, hU - hD, 2.0f);
            vertices[idx].Bitangent = glm::normalize(tangentZ);
        }
    }
}

void Terrain::loadTextures() {
    const std::string texPath = "/home/holmberg/development/Scilla/assets/textures/terrain/";

    m_grassTexture   = Texture(texPath + "Grass001_2K-PNG_Color.png", "texture_diffuse", true, false);
    m_grassNormal    = Texture(texPath + "Grass001_2K-PNG_NormalGL.png", "texture_normal", true, false);
    m_grassRoughness = Texture(texPath + "Grass001_2K-PNG_Roughness.png", "texture_roughness", false, false);
    m_grassAO        = Texture(texPath + "Grass001_2K-PNG_AmbientOcclusion.png", "texture_ao", false, false);

    m_rockTexture   = Texture(texPath + "rock_face_03_diff_2k.jpg", "texture_diffuse", true, false);
    m_rockNormal    = Texture(texPath + "rock_face_03_nor_gl_2k.png", "texture_normal", true, false);
    m_rockAO        = Texture(texPath + "rock_face_03_ao_2k.png", "texture_ao", false, false);
    m_rockRoughness = Texture(texPath + "rock_face_03_rough_2k.png", "texture_roughness", false, false);

    m_snowTexture   = Texture(texPath + "snow_field_aerial_col_2k.jpg", "texture_diffuse", true, false);
    m_snowNormal    = Texture(texPath + "snow_field_aerial_nor_gl_2k.png", "texture_normal", true, false);
    m_snowAO        = Texture(texPath + "snow_field_aerial_ao_2k.png", "texture_ao", false, false);
    m_snowRoughness = Texture(texPath + "snow_field_aerial_rough_2k.png", "texture_roughness", false, false);
}

void Terrain::setupMesh(const std::vector<TerrainVertex>& vertices, const std::vector<unsigned int>& indices) {
    m_VAO.generate();
    m_VBO.generate();
    m_EBO.generate();

    m_VBO.setData(vertices.data(), vertices.size() * sizeof(TerrainVertex));
    m_EBO.setData(indices.data(), indices.size() * sizeof(unsigned int));

    m_VAO.bindVBO(m_VBO, 0, sizeof(TerrainVertex));
    m_VAO.bindEBO(m_EBO);

    // Position
    m_VAO.enableAttrib(0);
    m_VAO.setAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Position), 0);

    // Normal
    m_VAO.enableAttrib(1);
    m_VAO.setAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Normal), 0);

    // TexCoords
    m_VAO.enableAttrib(2);
    m_VAO.setAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, TexCoords), 0);

    // Tangent
    m_VAO.enableAttrib(3);
    m_VAO.setAttribFormat(3, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Tangent), 0);

    // Bitangent
    m_VAO.enableAttrib(4);
    m_VAO.setAttribFormat(4, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Bitangent), 0);
}

void Terrain::render(const Shader& shader) const {
    m_grassTexture.bindToTextureUnit(0);
    m_grassNormal.bindToTextureUnit(1);
    m_grassRoughness.bindToTextureUnit(2);
    m_grassAO.bindToTextureUnit(3);

    m_rockTexture.bindToTextureUnit(4);
    m_rockNormal.bindToTextureUnit(5);
    m_rockRoughness.bindToTextureUnit(6);
    m_rockAO.bindToTextureUnit(7);

    m_snowTexture.bindToTextureUnit(8);
    m_snowNormal.bindToTextureUnit(9);
    m_snowRoughness.bindToTextureUnit(10);
    m_snowAO.bindToTextureUnit(11);

    shader.use();

    shader.setTextureUnit("grassTexture", 0);
    shader.setTextureUnit("grassNormal", 1);
    shader.setTextureUnit("grassRoughness", 2);
    shader.setTextureUnit("grassAO", 3);

    shader.setTextureUnit("rockTexture", 4);
    shader.setTextureUnit("rockNormal", 5);
    shader.setTextureUnit("rockRoughness", 6);
    shader.setTextureUnit("rockAO", 7);

    shader.setTextureUnit("snowTexture", 8);
    shader.setTextureUnit("snowNormal", 9);
    shader.setTextureUnit("snowRoughness", 10);
    shader.setTextureUnit("snowAO", 11);

    m_VAO.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}

glm::mat4 Terrain::getModelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, m_position);
    return model;
}
