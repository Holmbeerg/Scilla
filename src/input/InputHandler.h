#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class InputHandler {
public:
    explicit InputHandler(GLFWwindow* window);
    [[nodiscard]] GLFWwindow* getWindow() const { return m_window; }
    void handleKeyInput(int key, int scancode, int action, int mods);
    [[nodiscard]] bool isNormalMappingEnabled() const { return m_normalMapping; }
    [[nodiscard]] bool shouldReloadShaders() const { return m_shouldReload; }
    void resetReloadFlag() { m_shouldReload = false; }
    void toggleCursorVisibility();

private:
    void toggleWireframeMode();
    GLFWwindow* m_window;
    bool m_wireframeMode;
    bool m_normalMapping;
    bool m_cursorLocked = true;
    bool m_shouldReload = false;
};
