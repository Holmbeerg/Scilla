#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    void update(double xpos, double ypos);
    void processInput(GLFWwindow *window, float deltaTime);
    void processScroll(double xoffset, double yoffset);
    [[nodiscard]] glm::vec3 getCameraPos() const;
    void setPosition(const glm::vec3& position);
    void setPosition(float x, float y, float z);
    [[nodiscard]] float getFov() const;
    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix(float width, float height) const;
    void setFirstMouse(const bool b) { m_firstMouse = b; }
    void setCameraPos(const glm::vec3& pos) { m_cameraPos = pos; updateView(); }
private:
    float m_yaw;
    float m_pitch;
    float m_roll;
    float m_lastX;
    float m_lastY;
    float m_xOffset;
    float m_yOffset;
    float m_sensitivity;
    float m_speed;
    float m_fov;
    bool m_firstMouse;
    glm::vec3 m_direction;
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraFront;
    glm::vec3 m_cameraUp;
    glm::vec3 m_cameraRight;
    glm::vec3 m_worldUp;
    glm::mat4 m_view;
    void updateView();
};
