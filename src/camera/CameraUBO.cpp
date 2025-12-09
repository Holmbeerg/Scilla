#include "CameraUBO.h"

// Constants
constexpr unsigned int CAMERA_BINDING_POINT = 0;

CameraUBO::CameraUBO() 
    : m_buffer(CAMERA_BINDING_POINT, sizeof(CameraData)) 
{
    // The m_buffer constructor (in UniformBuffer.cpp) handles the 
    // creation and binding of the UBO.
}

void CameraUBO::setViewProjection(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) const {
    CameraData data{};
    data.view = view;
    data.projection = projection;
    data.viewPos = glm::vec4(viewPos, 1.0f); // vec4 for std140 alignment

    // Offset 0, size of struct, and pointer to the local data.
    m_buffer.updateData(0, sizeof(CameraData), &data);
}