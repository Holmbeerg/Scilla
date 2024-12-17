#include <glad/glad.h>
#include "EBO.h"

EBO::EBO()
    : m_id(0) { }

void EBO::generate() {
    glGenBuffers(1, &m_id);
}

void EBO::setData(const GLuint* indices, GLsizeiptr size, GLenum usage) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);  // Upload index data to the buffer
}

void EBO::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);  // Bind the element array buffer
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // Unbind the element array buffer
}

GLuint EBO::getID() const {
    return m_id;  // Return the buffer ID
}

EBO::~EBO() {
    glDeleteBuffers(1, &m_id);  // Delete the buffer when the object is destroyed
}

