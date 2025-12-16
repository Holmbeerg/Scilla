#pragma once
#include <glm/glm.hpp>
#include "../graphics/Shader.h"
#include "graphics/buffers/VAO.h"
#include "graphics/buffers/VBO.h"

class Skybox {
public:
    explicit Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void render(const Shader &shader, const glm::vec3 &sunDir, float dayTime) const;

private:
    VAO m_VAO;
    VBO m_VBO;
};
