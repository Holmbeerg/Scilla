#pragma once
#include <glm/glm.hpp>

#include "graphics/buffers/UniformBuffer.h"

class CameraUBO {
public:
    CameraUBO();
    ~CameraUBO() = default;

    void setViewProjection(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) const;

private:
    // The raw data structure that matches the GPU memory layout (std140)
    struct CameraData {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec4 viewPos;
    };

    // Composition: The CameraUBO "owns" the underlying GPU buffer
    UniformBuffer m_buffer;
};