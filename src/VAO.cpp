#include "VAO.h"

VAO::VAO()
    : m_id(0) {
}

void VAO::generate() {
    glGenVertexArrays(1, &m_id);
}

void VAO::bind() const { // const: should not modify state of object
    glBindVertexArray(m_id);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

[[nodiscard]] GLuint VAO::getID() const {
    return m_id;
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &m_id);
}