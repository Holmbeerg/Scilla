// EBO.h
#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

class EBO {

public:
    EBO();
    void generate();
    void setData(const GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    void bind() const;
    static void unbind();
    [[nodiscard]] GLuint getID() const;
    ~EBO();

private:
    GLuint m_id;
};

#endif