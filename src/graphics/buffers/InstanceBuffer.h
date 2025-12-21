#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class InstanceBuffer {
public:
    InstanceBuffer();
    ~InstanceBuffer();

    void generate();

    void addData(const glm::mat4& matrix);

    void setData() const;

    [[nodiscard]] GLuint getID() const;
    [[nodiscard]] GLsizei getCount() const;

private:
    GLuint m_bufferID;
    std::vector<glm::mat4> m_data;
};