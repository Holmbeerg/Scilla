#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer() : m_bufferID(0) { }

InstanceBuffer::~InstanceBuffer() {
    if (m_bufferID != 0) {
        glDeleteBuffers(1, &m_bufferID);
    }
}

void InstanceBuffer::generate() {
    if (m_bufferID == 0) {
        glCreateBuffers(1, &m_bufferID);
    }
}

void InstanceBuffer::addData(const glm::mat4& matrix) { // set the model matrix for an instance. Position, rotation, scale combined.
    m_data.push_back(matrix);
}

void InstanceBuffer::setData() const {
    if (m_data.empty() || m_bufferID == 0) return;

    glNamedBufferStorage(
        m_bufferID,
        m_data.size() * sizeof(glm::mat4),
        m_data.data(),
        GL_DYNAMIC_STORAGE_BIT
    );
}

GLuint InstanceBuffer::getID() const {
    return m_bufferID;
}

GLsizei InstanceBuffer::getCount() const {
    return static_cast<GLsizei>(m_data.size());
}