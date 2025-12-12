#include "Terrain.h"
#include <iostream>

Terrain::Terrain(const int width, const int depth, const std::vector<float>& heightMap)
    : m_heights(heightMap), m_VAO(0), m_VBO(0), m_EBO(0), m_indexCount(0), m_width(width), m_depth(depth) {

    std::vector<TerrainVertex> vertices;
    std::vector<unsigned int> indices;

    // 1. BUILD VERTICES
    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float y = heightMap[z * width + x];

            TerrainVertex v{};
            v.Position = glm::vec3(x, y, z);
            v.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Will calculate proper normals below
            v.TexCoords = glm::vec2(static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f);

            vertices.push_back(v);
        }
    }

    // https://stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid
    // 2. CALCULATE PROPER NORMALS - Finite Difference Method
    for (int z = 1; z < depth - 1; z++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = z * width + x;

            float hL = heightMap[z * width + (x - 1)];
            float hR = heightMap[z * width + (x + 1)];
            float hD = heightMap[(z - 1) * width + x];
            float hU = heightMap[(z + 1) * width + x];

            // Calculate normal using finite difference
            glm::vec3 normal;
            normal.x = hL - hR;
            normal.y = 2.0f; // Y is up
            normal.z = hD - hU;
            vertices[idx].Normal = glm::normalize(normal);

            glm::vec3 tangentX(2.0f, hR - hL, 0.0f);
            vertices[idx].Tangent = glm::normalize(tangentX); // tangent and bitangent are needed for normal mapping, transform normal map from tangent space to world space

            glm::vec3 tangentZ(0.0f, hU - hD, 2.0f);
            vertices[idx].Bitangent = glm::normalize(tangentZ);
        }
    }

    // 3. BUILD INDICES (Quads made of 2 triangles)
    for (int z = 0; z < depth - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            const int topLeft = (z * width) + x;
            const int topRight = topLeft + 1;
            const int bottomLeft = ((z + 1) * width) + x;
            const int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_indexCount = static_cast<GLsizei>(indices.size());

    // 4. UPLOAD
    setupMesh(vertices, indices);

    // 5. LOAD TEXTURES (Albedo + Normal maps)
    const std::string texPath = "/home/holmberg/development/Scilla/assets/textures/"; // TODO: Make relative path

    m_grassTexture = Texture(texPath + "coast_sand_rocks_02_diff_2k.jpg", "texture_diffuse", true, false);
    m_grassNormal  = Texture(texPath + "coast_sand_rocks_02_nor_gl_2k.png", "texture_normal", true, false);

    m_rockTexture  = Texture(texPath + "rock_face_03_diff_2k.jpg", "texture_diffuse", true, false);
    m_rockNormal   = Texture(texPath + "rock_face_03_nor_gl_2k.png", "texture_normal", true, false);

    m_snowTexture  = Texture(texPath + "snow_field_aerial_col_2k.jpg", "texture_diffuse", true, false);
    m_snowNormal   = Texture(texPath + "snow_field_aerial_nor_gl_2k.png", "texture_normal", true, false);
}


void Terrain::setupMesh(const std::vector<TerrainVertex>& vertices, const std::vector<unsigned int>& indices) {
    glCreateVertexArrays(1, &m_VAO);
    glCreateBuffers(1, &m_VBO);
    glCreateBuffers(1, &m_EBO);

    glNamedBufferStorage(m_VBO, sizeof(TerrainVertex) * vertices.size(), vertices.data(), 0);
    glNamedBufferStorage(m_EBO, sizeof(unsigned int) * indices.size(), indices.data(), 0);

    // Link buffers to VAO
    glVertexArrayElementBuffer(m_VAO, m_EBO);
    // Binding Index 0 is linked to m_VBO, with stride sizeof(TerrainVertex)
    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(TerrainVertex));

    // Position
    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Position));
    glVertexArrayAttribBinding(m_VAO, 0, 0);

    // Normal
    glEnableVertexArrayAttrib(m_VAO, 1);
    glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Normal));
    glVertexArrayAttribBinding(m_VAO, 1, 0);

    // TexCoords (UV)
    glEnableVertexArrayAttrib(m_VAO, 2);
    glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, TexCoords));
    glVertexArrayAttribBinding(m_VAO, 2, 0);

    // Tangent
    glEnableVertexArrayAttrib(m_VAO, 3);
    glVertexArrayAttribFormat(m_VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Tangent));
    glVertexArrayAttribBinding(m_VAO, 3, 0);

    // Bitangent
    glEnableVertexArrayAttrib(m_VAO, 4);
    glVertexArrayAttribFormat(m_VAO, 4, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, Bitangent));
    glVertexArrayAttribBinding(m_VAO, 4, 0);
}
void Terrain::render(const Shader& shader) const {
    // Bind albedo textures
    m_grassTexture.bindToTextureUnit(0);
    m_rockTexture.bindToTextureUnit(1);
    m_snowTexture.bindToTextureUnit(2);

    // Bind normal maps
    m_grassNormal.bindToTextureUnit(3);
    m_rockNormal.bindToTextureUnit(4);
    m_snowNormal.bindToTextureUnit(5);

    // Set uniforms
    shader.setTextureUnit("grassTexture", 0);
    shader.setTextureUnit("rockTexture", 1);
    shader.setTextureUnit("snowTexture", 2);
    shader.setTextureUnit("grassNormal", 3);
    shader.setTextureUnit("rockNormal", 4);
    shader.setTextureUnit("snowNormal", 5);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}