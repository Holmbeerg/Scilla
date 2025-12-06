#pragma once
#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Shader.h"
#include "Texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures);

    void Draw(const Shader &shader) const;

    [[nodiscard]] const std::vector<Vertex> &getVertices() const { return m_vertices; } // return by reference to avoid copying
    [[nodiscard]] const std::vector<unsigned int> &getIndices() const { return m_indices; }
    [[nodiscard]] const std::vector<std::shared_ptr<Texture>> &getTextures() const { return m_textures; }

    [[nodiscard]] unsigned int getVAO() const { return m_VAO; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<std::shared_ptr<Texture>> m_textures;

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;

    void setupMesh();
};
