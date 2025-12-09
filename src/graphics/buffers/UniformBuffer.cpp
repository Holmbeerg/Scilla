// The UniformBuffer class encapsulates the creation and management of Uniform Buffer Objects (UBOs).

#include "UniformBuffer.h"
#include <glad/glad.h>

UniformBuffer::UniformBuffer(const unsigned int binding, const size_t size)
    : ubo(0), bindingPoint(binding), size(size)
{
    glCreateBuffers(1, &ubo);
    glNamedBufferStorage(ubo, size, nullptr, GL_DYNAMIC_STORAGE_BIT); // immutable, prefer this over glNamedBufferData
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo); // Bind the UBO to the specified binding point which can be accessed in shaders
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &ubo);
}

void UniformBuffer::updateData(const size_t offset, const size_t dataSize, const void* data) const {
    glNamedBufferSubData(ubo, offset, dataSize, data);
}