#pragma once
#include <glad.h>

class VBO {

public:
    VBO();

    void generate();

    void setData(const void* data, GLsizeiptr size) const;

    [[nodiscard]] GLuint getID() const;

    ~VBO();

private:
    GLuint m_vboID;
};