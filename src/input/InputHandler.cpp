#include "InputHandler.h"

#include <iostream>
#include <ostream>


InputHandler::InputHandler(GLFWwindow *window)
    : m_window(window)
      , m_wireframeMode(false),
      m_normalMapping(true) {
    // Store the instance pointer
    glfwSetWindowUserPointer(window, this);
    // https://stackoverflow.com/questions/55145966/what-does-glfwgetwindowuserpointer-do
    glfwSetKeyCallback(window, keyCallback);
}

void InputHandler::keyCallback(GLFWwindow *window, const int key, const int scancode, const int action,
                               const int mods) {
    if (auto *handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window))) {
        // Retrieve the instance pointer
        handler->handleKeyInput(key, scancode, action, mods);
    }
}

void InputHandler::handleKeyInput(const int key, int scancode, const int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }

    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        toggleCursorVisibility();
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        toggleWireframeMode();
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        m_normalMapping = !m_normalMapping;
        std::cout << "Normal Mapping: " << (m_normalMapping ? "ON" : "OFF") << std::endl;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        m_shouldReload = true;
    }
}

void InputHandler::toggleWireframeMode() {
    m_wireframeMode = !m_wireframeMode;
    if (m_wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void InputHandler::toggleCursorVisibility() {
    m_cursorLocked = !m_cursorLocked;

    if (m_cursorLocked) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
