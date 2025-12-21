#include "InstancedModel.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

InstancedModel::InstancedModel(std::shared_ptr<Model> model)
    : m_model(std::move(model)) {
}

// we only rotate around Y axis because trees/plants.
// to look varied but not topple over.
void InstancedModel::addInstance(const glm::vec3& position, const glm::vec3& scale, const float rotationY) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0, 1, 0));
    model = glm::scale(model, scale);

    m_buffer.addData(model);
}

void InstancedModel::finalize() {
    m_buffer.generate();

    m_buffer.setData();

    for (const auto& meshes = m_model->getMeshes(); const auto& mesh : meshes) {
        const auto& vao = mesh.getVAO();
        const GLuint vaoID = vao.getID();
        const GLuint bufferID = m_buffer.getID();

        // Bind the InstanceBuffer to Binding Point 1. The VBO is at Binding Point 0.
        glVertexArrayVertexBuffer(vaoID, 1, bufferID, 0, sizeof(glm::mat4));

        // Set the Divisor to 1
        // This tells OpenGL: "Advance the data in Binding Point 1 once per INSTANCE"
        vao.setBindingDivisor(1, 1);

        // A mat4 is too big for one attribute slot. It takes up 4 slots of vec4.
        // locations 0, 1, 2 are taken by Position, Normal, TexCoord.
        // We use locations 3, 4, 5, 6 for the Matrix.
        for (int i = 0; i < 4; i++) {
            const GLuint loc = 3 + i; // Locations 3, 4, 5, 6 in the VAO

            vao.enableAttrib(loc);

            const GLuint offset = i * sizeof(glm::vec4); // Offset for each column in the mat4. distance between columns.
            vao.setAttribFormat(loc, 4, GL_FLOAT, GL_FALSE, offset, 1);
        }
    }
}