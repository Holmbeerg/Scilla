#include "VBO.h"

#include <glad/glad.h>

VBO::VBO()
    : m_vboID(0) { }

void VBO::generate() {
    glCreateBuffers(1, &m_vboID);  // Generate the buffer
}

void VBO::setData(const GLfloat *vertices, const GLsizeiptr size) const {
    // Use glNamedBufferStorage to allocate immutable buffer
    glNamedBufferStorage(m_vboID, size, vertices, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

[[nodiscard]] GLuint VBO::getID() const {
    return m_vboID;
}

VBO::~VBO() {
    glDeleteBuffers(1, &m_vboID);
}
