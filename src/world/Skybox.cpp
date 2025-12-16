#include "Skybox.h"

#include <cmath>
#include <iostream>
#include <ostream>

static constexpr float skyboxVertices[] = {
    // Back face
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    // Left face
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    // Right face
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    // Front face
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    // Top face
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

Skybox::Skybox() {
    m_VAO.generate();
    m_VBO.generate();

    m_VBO.setData(skyboxVertices, sizeof(skyboxVertices));
    m_VAO.bindVBO(m_VBO, 0, 3 * sizeof(float));

    m_VAO.enableAttrib(0);
    m_VAO.setAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Skybox::render(const Shader &shader, const glm::vec3 &sunDir, const float dayTime) const {
    glDepthFunc(GL_LEQUAL);

    shader.use();
    shader.setVec3("u_SunDirection", sunDir);
    shader.setFloat("u_Time", dayTime);

    m_VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}
