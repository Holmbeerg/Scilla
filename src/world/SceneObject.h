#pragma once
#include <memory>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

class Model;

struct SceneObject {
    std::shared_ptr<Model> model;
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    [[nodiscard]] glm::mat4 getTransform() const { // Compute the model matrix
        auto mat = glm::mat4(1.0f);
        mat = glm::translate(mat, position);

        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));

        mat = glm::scale(mat, scale);
        return mat;
    }
};