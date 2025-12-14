// The mesh handles the geometry data (vertices, indices)

#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           Material material)
    : m_vertices(std::move(vertices)), // we avoid an extra copy by moving
      m_indices(std::move(indices)),
      m_material(std::move(material)),
      m_VAO(0), m_VBO(0), m_EBO(0) {
    setupMesh();
}

void Mesh::setupMesh() {
    glCreateVertexArrays(1, &m_VAO);
    glCreateBuffers(1, &m_VBO);
    glCreateBuffers(1, &m_EBO);

    // Allocate immutable storage for buffers and upload data
    // GL_DYNAMIC_STORAGE_BIT allows updating data later with glNamedBufferSubData
    glNamedBufferStorage(m_VBO, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(m_EBO, sizeof(GLuint) * m_indices.size(), m_indices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Attach the element buffer (indices) to the VAO
    glVertexArrayElementBuffer(m_VAO, m_EBO);
    // Bind vertex buffer to the VAO at binding index 0
    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(Vertex));

    // Enable vertex attribute locations 0, 1, 2 and 3
    glEnableVertexArrayAttrib(m_VAO, 0); // Position
    glEnableVertexArrayAttrib(m_VAO, 1); // Normal
    glEnableVertexArrayAttrib(m_VAO, 2); // TexCoords
    glEnableVertexArrayAttrib(m_VAO, 3); // Tangent

    // Specify the format of each vertex attribute
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
    glVertexArrayAttribFormat(m_VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Tangent));

    // Link attribute locations (0, 1, 2, 3) to binding index 0
    glVertexArrayAttribBinding(m_VAO, 0, 0);
    glVertexArrayAttribBinding(m_VAO, 1, 0);
    glVertexArrayAttribBinding(m_VAO, 2, 0);
    glVertexArrayAttribBinding(m_VAO, 3, 0);
}

void Mesh::render(const Shader &shader) const {
    // 1. Bind Material
    // This handles all the textures (diffuse, specular, normal)
    // AND the shininess float automatically.
    m_material.bind(shader);

    // 2. Draw Geometry
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}
