#pragma once

#include <glad/glad.h>

class VBO; // faster than #include
class EBO;

class VAO {
    GLuint m_vaoID;

public:
    VAO();

    //
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) noexcept : m_vaoID(other.m_vaoID) {
        other.m_vaoID = 0;
    }

    VAO& operator=(VAO&& other) noexcept {
        if (this != &other) {
            if (m_vaoID) glDeleteVertexArrays(1, &m_vaoID); // Clean up current
            m_vaoID = other.m_vaoID; // Steal ID
            other.m_vaoID = 0;       // Nullify source
        }
        return *this;
    }

    void generate();

    void bind() const;

    void bindVBO(const VBO& vbo, GLuint bindingIndex, GLsizei stride) const;

    void bindEBO(const EBO& ebo) const;

    void enableAttrib(GLuint index) const;

    void disableAttrib(GLuint index) const;

    void setAttribFormat(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset, GLuint bindingIndex) const;

    void setBindingDivisor(GLuint bindingIndex, GLuint divisor) const;

    [[nodiscard]] GLuint getID() const;

    ~VAO();
};

