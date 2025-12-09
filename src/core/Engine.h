#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Scene.h"
#include "input/InputHandler.h"
#include "utils/FrameTimer.h"

class Engine {
public:
    Engine() = default;

    bool initialize(int width, int height, const char* title);
    void run();
    static void shutdown();

    // Prevent copying
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Callbacks need access to these
    void handleResize(int width, int height) const;
    void handleMouse(double xpos, double ypos) const;
    void handleScroll(double xoffset, double yoffset) const;
    static void keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    GLFWwindow* m_window = nullptr;

    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<InputHandler> m_inputHandler;
    std::unique_ptr<Renderer> m_renderer;

    FrameTimer m_frameTimer;
};