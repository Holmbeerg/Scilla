#include "Camera.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera()
    : m_yaw(0.0f), m_pitch(0.0f), m_roll(0.0f),
      m_lastX(400.0f), m_lastY(300.0f),
      m_xOffset(0.0f), m_yOffset(0.0f),
      m_sensitivity(0.1f), m_speed(4.0f),
      m_fov(45.0f), m_firstMouse(true),
      m_direction(0.0f, 0.0f, 0.0f), m_cameraPos(0.0f, 0.0f, 3.0f), m_cameraFront(0.0f, 0.0f, -3.0f),
      m_cameraUp(0.0f, 1.0f, 0.0f), m_cameraRight(0.0f, 0.0f, 0.0f), m_worldUp(0.0f, 1.0f, 0.0f),
      m_view(glm::mat4(1.0f)) {
    updateView();
}

void Camera::update(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    m_xOffset = xpos - m_lastX;
    m_yOffset = m_lastY - ypos;

    m_lastX = xpos;
    m_lastY = ypos;

    m_xOffset *= m_sensitivity;
    m_yOffset *= m_sensitivity;

    m_yaw += m_xOffset;
    m_pitch += m_yOffset;

    if (m_pitch > 89.0f) {
        m_pitch = 89.0f;
    }
    if (m_pitch < -89.0f) {
        m_pitch = -89.0f;
    }
    updateView();
}

void Camera::updateView() {
    m_direction.x = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_direction.y = sin(glm::radians(m_pitch));
    m_direction.z = -cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_cameraFront = normalize(m_direction);

    m_cameraRight = normalize(cross(m_cameraFront, m_worldUp));
    m_cameraUp = cross(m_cameraRight, m_cameraFront);

    m_view = lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
}

void Camera::processInput(GLFWwindow *window, const float deltaTime) {
    float cameraSpeed = m_speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 3.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_cameraPos += cameraSpeed * m_cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_cameraPos -= cameraSpeed * m_cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_cameraPos -= normalize(cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_cameraPos += normalize(cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
    }
    updateView();
}

void Camera::processScroll(double xoffset, double yoffset) {
    m_fov -= static_cast<float>(yoffset);

    if (m_fov < 1.0f) {
        m_fov = 1.0f;
    }
    if (m_fov > 45.0f) {
        m_fov = 45.0f;
    }
}

float Camera::getFov() const {
    return m_fov;
}

glm::mat4 Camera::getViewMatrix() const {
    return m_view;
}
