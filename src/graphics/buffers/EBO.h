#pragma once
#include <glad/glad.h>

class EBO {

public:
    EBO();

    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;

    EBO(EBO&& other) noexcept : m_eboID(other.m_eboID) {
        other.m_eboID = 0;
    }

    // Add Move Assignment
    EBO& operator=(EBO&& other) noexcept {
        if (this != &other) {
            if (m_eboID) glDeleteBuffers(1, &m_eboID); // Clean up current
            m_eboID = other.m_eboID; // Steal ID
            other.m_eboID = 0;       // Nullify source
        }
        return *this;
    }

    void generate();
    void setData(const GLuint* indices, GLsizeiptr size) const;
    [[nodiscard]] GLuint getID() const;
    ~EBO();

private:
    GLuint m_eboID;
};
