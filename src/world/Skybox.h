#pragma once
#include <string>
#include <vector>
#include <glm/fwd.hpp>

#include "../graphics/Shader.h"


class Skybox {
public:
    explicit Skybox(const std::vector<std::string> &faces);
    void render(const Shader &shader, const glm::mat4 &view, const glm::mat4 &projection) const;
private:
    unsigned int m_cubemapTexture;
    unsigned int m_VAO, m_VBO;

    static unsigned int loadCubemap(const std::vector<std::string> &faces);
};
