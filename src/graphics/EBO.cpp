#include <glad.h>
#include "EBO.h"

EBO::EBO()
    : m_eboID(0) { }

void EBO::generate() {
    glCreateBuffers(1, &m_eboID);
}

void EBO::setData(const GLuint* indices, const GLsizeiptr size) const {
    glNamedBufferStorage(m_eboID, size, indices, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

GLuint EBO::getID() const {
    return m_eboID;  // Return the buffer ID
}

EBO::~EBO() {
    glDeleteBuffers(1, &m_eboID);  // Delete the buffer when the object is destroyed
}

