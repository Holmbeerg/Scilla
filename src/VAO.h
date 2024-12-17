#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>

class VAO {
    GLuint m_id;

public:
    VAO();

    void generate();

    void bind() const;

    static void unbind();

    [[nodiscard]] GLuint getID() const;

    ~VAO();
};
#endif
