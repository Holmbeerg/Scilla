#pragma once
#include <cstddef> // for size_t
#include <glad/glad.h>

class UniformBuffer {
    unsigned int ubo;
    unsigned int bindingPoint;
    size_t size;

public:
    UniformBuffer(unsigned int binding, size_t size);

    ~UniformBuffer();

    // Disable copying
    UniformBuffer(const UniformBuffer &) = delete;

    UniformBuffer &operator=(const UniformBuffer &) = delete;

    // Allow moving
    UniformBuffer(UniformBuffer &&other) noexcept
        : ubo(other.ubo), bindingPoint(other.bindingPoint), size(other.size) {
        other.ubo = 0;
    }

    UniformBuffer &operator=(UniformBuffer &&other) noexcept {
        // correct?
        if (this != &other) {
            glDeleteBuffers(1, &ubo); // Clean up our current buffer
            ubo = other.ubo; // Steal theirs
            bindingPoint = other.bindingPoint;
            size = other.size;
            other.ubo = 0; // Nullify theirs
        }
        return *this;
    }

    void updateData(size_t offset, size_t dataSize, const void *data) const;
};
