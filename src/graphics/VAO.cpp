#include "VAO.h"

#include "VBO.h"
#include "EBO.h"

VAO::VAO() : m_vaoID(0) { }

void VAO::generate() {
    glCreateVertexArrays(1, &m_vaoID);
}

void VAO::bind() const {
    glBindVertexArray(m_vaoID);
}

void VAO::bindVBO(const VBO &vbo, const GLuint bindingIndex, const GLsizei stride) const {
    glVertexArrayVertexBuffer(m_vaoID, bindingIndex, vbo.getID(), 0, stride);
}

void VAO::bindEBO(EBO &ebo) const {
    glVertexArrayElementBuffer(m_vaoID, ebo.getID());
}

void VAO::enableAttrib(const GLuint index) const {
    glEnableVertexArrayAttrib(m_vaoID, index);
}

void VAO::disableAttrib(const GLuint index) const {
    glDisableVertexArrayAttrib(m_vaoID, index);
}

void VAO::setAttribFormat(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset, const GLuint bindingIndex) const {
    glVertexArrayAttribFormat(m_vaoID, index, size, type, normalized, offset);
    glVertexArrayAttribBinding(m_vaoID, index, bindingIndex);
}

[[nodiscard]] GLuint VAO::getID() const {
    return m_vaoID;
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &m_vaoID);
}
