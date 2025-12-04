#pragma once

#include <glad/glad.h>

class VBO; // faster than #include
class EBO;

class VAO {
    GLuint m_vaoID;

public:
    VAO();

    void generate();

    void bind() const;

    void bindVBO(const VBO& vbo, GLuint bindingIndex, GLsizei stride) const;

    void bindEBO(const EBO& ebo) const;

    void enableAttrib(GLuint index) const;

    void disableAttrib(GLuint index) const;

    void setAttribFormat(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset, GLuint bindingIndex) const;

    [[nodiscard]] GLuint getID() const;

    ~VAO();
};

