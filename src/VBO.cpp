#include "VBO.h"

#include <glad/glad.h>

VBO::VBO()
    : m_id(0) { }

void VBO::generate() {
    glGenBuffers(1, &m_id);  // Generate the buffer
}

void VBO::setData(const GLfloat *vertices, const GLsizeiptr size, const GLenum usage) {
    glNamedBufferData(m_id, size, vertices, usage);
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBO::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

[[nodiscard]] GLuint VBO::getID() const {
    return m_id;
}

VBO::~VBO() {
    glDeleteBuffers(1, &m_id);
}
