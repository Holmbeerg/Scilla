#pragma once
#include <glad/glad.h>

class VBO {

public:
    VBO();

    void generate();

    void setData(const GLfloat *vertices, const GLsizeiptr size) const;

    void bind() const;

    static void unbind();

    [[nodiscard]] GLuint getID() const;

    ~VBO();

private:
    GLuint m_vboID;
};