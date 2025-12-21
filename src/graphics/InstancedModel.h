// graphics/InstancedModel.h
#pragma once
#include <memory>
#include <vector>
#include "Model.h"
#include "buffers/InstanceBuffer.h"

class InstancedModel {
public:
    explicit InstancedModel(std::shared_ptr<Model> model);

    void addInstance(const glm::vec3& position, const glm::vec3& scale, float rotationY);
    void finalize(); // Generates buffers, uploads data, configures VAO

    [[nodiscard]] const std::shared_ptr<Model>& getModel() const { return m_model; }
    [[nodiscard]] GLsizei getInstanceCount() const { return m_buffer.getCount(); }

private:
    std::shared_ptr<Model> m_model;
    InstanceBuffer m_buffer;
};