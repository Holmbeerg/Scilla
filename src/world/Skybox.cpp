#include "Skybox.h"

#include <cmath>
#include <iostream>
#include <ostream>

#include "stb_image.h"

static constexpr float skyboxVertices[] = {
    // Back face
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    // Left face
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    // Right face
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    // Front face
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox()
    : m_VAO(0), m_VBO(0) {
    glCreateVertexArrays(1, &m_VAO);

    glCreateBuffers(1, &m_VBO);
    glNamedBufferStorage(m_VBO, sizeof(skyboxVertices), skyboxVertices, 0);

    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, 3 * sizeof(float));

    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO, 0, 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Skybox::update(const float deltaTime) {
    m_dayTime += deltaTime * 0.1; // Adjust 0.1f to change day speed

    // Simple math to make the sun go round in a circle
    m_sunDirection.x = std::cos(m_dayTime);
    m_sunDirection.y = std::sin(m_dayTime);
    m_sunDirection.z = 0.0f;

    // Normalize so it's always length 1.0
    m_sunDirection = glm::normalize(m_sunDirection);
}

void Skybox::render(const Shader &shader) const {
    glDepthFunc(GL_LEQUAL);  // Change depth function to LEQUAL because the skybox is exactly at depth 1.0
    shader.use();

    shader.setVec3("u_SunDirection", m_sunDirection);
    shader.setFloat("u_Time", m_dayTime);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS); // set depth function back to default
}