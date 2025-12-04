#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    : m_vertices(std::move(vertices)), m_indices(std::move(indices)), m_textures(std::move(textures)) { // we avoid an extra copy by moving
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

    // Enable vertex attribute locations 0, 1, and 2
    glEnableVertexArrayAttrib(m_VAO, 0); // Position
    glEnableVertexArrayAttrib(m_VAO, 1); // Normal
    glEnableVertexArrayAttrib(m_VAO, 2); // TexCoords

    // Specify the format of each vertex attribute
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));

    // Link attribute locations (0, 1, 2) to binding index 0
    glVertexArrayAttribBinding(m_VAO, 0, 0);
    glVertexArrayAttribBinding(m_VAO, 1, 0);
    glVertexArrayAttribBinding(m_VAO, 2, 0);
}

void Mesh::Draw(const Shader &shader) const {
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;

    for(size_t i = 0; i < m_textures.size(); i++) {
        std::string number;
        std::string name = m_textures[i].type;

        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt("material." + name + number, static_cast<int>(i));

        glBindTextureUnit(i, m_textures[i].id);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}
