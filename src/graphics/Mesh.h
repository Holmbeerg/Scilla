#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Material.h"
#include "Shader.h"
#include "buffers/EBO.h"
#include "buffers/VAO.h"
#include "buffers/VBO.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);

    void render(const Shader &shader) const;

    [[nodiscard]] const VAO& getVAO() const { return m_VAO; }
    [[nodiscard]] size_t getIndexCount() const { return m_indices.size(); }

    [[nodiscard]] const std::vector<Vertex> &getVertices() const { return m_vertices; } // return by reference to avoid copying
    [[nodiscard]] const std::vector<unsigned int> &getIndices() const { return m_indices; }
    [[nodiscard]] const Material &getMaterial() const { return m_material; }


private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    Material m_material;

    VAO m_VAO; // Move only
    VBO m_VBO; // Move only
    EBO m_EBO; // Move only

    void setupMesh();
};
