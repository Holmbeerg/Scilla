#pragma once
#include <glad.h>

class EBO {

public:
    EBO();
    void generate();
    void setData(const GLuint* indices, GLsizeiptr size) const;
    [[nodiscard]] GLuint getID() const;
    ~EBO();

private:
    GLuint m_eboID;
};
