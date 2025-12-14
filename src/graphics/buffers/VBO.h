#pragma once
#include <glad/glad.h>

class VBO {

public:
    VBO();

    // Delete Copy Constructor and Copy Assignment
    // We do not want to accidentally copy GPU resources
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    // Allow Move Constructor and Move Assignment
    // to transfer ownership of GPU resources
    VBO(VBO&& other) noexcept : m_vboID(other.m_vboID) {
        other.m_vboID = 0;
    }

    VBO& operator=(VBO&& other) noexcept {
        if (this != &other) {
            if (m_vboID) glDeleteBuffers(1, &m_vboID); // Clean up current
            m_vboID = other.m_vboID; // Steal ID
            other.m_vboID = 0;       // Nullify source
        }
        return *this;
    }

    void generate();

    void setData(const void* data, GLsizeiptr size) const;

    [[nodiscard]] GLuint getID() const;

    ~VBO();

private:
    GLuint m_vboID;
};