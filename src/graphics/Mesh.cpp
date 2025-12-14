// The mesh handles the geometry data (vertices, indices)

#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           Material material)
    : m_vertices(std::move(vertices)), // we avoid an extra copy by moving
      m_indices(std::move(indices)),
      m_material(std::move(material)) {
    setupMesh();
}

void Mesh::setupMesh() {
    m_VAO.generate();
    m_VBO.generate();
    m_EBO.generate();

    m_VBO.setData(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
    m_EBO.setData(m_indices.data(), m_indices.size() * sizeof(unsigned int));

    m_VAO.bindVBO(m_VBO, 0, sizeof(Vertex));

    m_VAO.bindEBO(m_EBO);

    m_VAO.enableAttrib(0);
    m_VAO.setAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position), 0);

    m_VAO.enableAttrib(1);
    m_VAO.setAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal), 0);

    m_VAO.enableAttrib(2);
    m_VAO.setAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords), 0);
}

void Mesh::render(const Shader &shader) const {
    // 1. Bind Material
    // This handles all the textures (diffuse, specular, normal)
    // AND the shininess float automatically.
    m_material.bind(shader);

    // 2. Draw Geometry
    m_VAO.bind(); // correct?
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}
