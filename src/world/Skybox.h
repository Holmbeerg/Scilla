#pragma once
#include <glm/glm.hpp>
#include "../graphics/Shader.h"

class Skybox {
public:
    explicit Skybox();
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void render(const Shader &shader, const glm::vec3 &sunDir, float dayTime) const;

private:
    unsigned int m_VAO, m_VBO;
};