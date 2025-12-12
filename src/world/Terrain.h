#pragma once
#include <vector>
#include <glad/glad.h>
#include "../graphics/Shader.h"
#include "../graphics/Texture.h"

class Terrain {
public:
    struct TerrainVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };
    Terrain(int width, int depth, const std::vector<float>& heightMap);
    void render(const Shader& shader) const;

private:
    void setupMesh(const std::vector<TerrainVertex>& vertices, const std::vector<unsigned int>& indices);

    std::vector<float> m_heights;

    GLuint m_VAO, m_VBO, m_EBO;
    GLsizei m_indexCount;
    int m_width, m_depth;

    Texture m_grassTexture;
    Texture m_rockTexture;
    Texture m_snowTexture;

    Texture m_grassNormal;
    Texture m_rockNormal;
    Texture m_snowNormal;
};




