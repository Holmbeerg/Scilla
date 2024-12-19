#pragma once
#include <GLFW/glfw3.h>

class InputHandler {
public:
    explicit InputHandler(GLFWwindow* window);
    void handleKeyInput(int key, int scancode, int action, int mods);

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void toggleWireframeMode();
    GLFWwindow* m_window;
    bool m_wireframeMode;
};
