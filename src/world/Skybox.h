#pragma once
#include <string>
#include <vector>
#include <glm/fwd.hpp>

#include "../graphics/Shader.h"


class Skybox {
public:
    explicit Skybox();

    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void update(float deltaTime);
    void render(const Shader &shader) const;
private:
    unsigned int m_VAO, m_VBO;
    float m_dayTime = 0.0f;
    glm::vec3 m_sunDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};
