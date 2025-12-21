// Engine is the main class that initializes and runs the rendering engine.
// The Engine manages the window, input handling, scene, and renderer.
// If we add physics or audio, those systems would also be managed here.

#include "Engine.h"
#include <iostream>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    if(auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window))) engine->handleResize(width, height);
}
void mouse_callback(GLFWwindow* window, const double xpos, const double ypos) {
    if(auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window))) engine->handleMouse(xpos, ypos);
}
void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
    if(auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window))) engine->handleScroll(xoffset, yoffset);
}

void Engine::keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window))) {
        engine->m_inputHandler->handleKeyInput(key, scancode, action, mods);
    }
}

bool Engine::initialize(const int width, const int height, const char* title) {
    std::cout << "Engine::initialize - this = " << this << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // 0 to disable V-Sync, 1 to enable
    glfwSetWindowUserPointer(m_window, this); // THERE CAN ONLY BE ONE!

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        return false;
    }

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetKeyCallback(m_window, keyCallbackStatic);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // enable docking

    ImGui::StyleColorsDark(); // Styling

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Initialize Components
    m_inputHandler = std::make_unique<InputHandler>(m_window);

    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();

    m_scene = std::make_unique<Scene>();
    m_scene->initialize();

    return true;
}

void Engine::run() {
    while (!glfwWindowShouldClose(m_window)) {
        m_frameTimer.update();

        if (m_inputHandler->shouldReloadShaders()) {
            m_renderer->reloadShaders();
            m_inputHandler->resetReloadFlag();
        }

        // Logic
        if (m_scene) {
            m_scene->update(m_frameTimer.getDeltaTime(), *m_inputHandler);
        }

        // Render
        if (m_renderer && m_scene) {
            m_renderer->render(*m_scene, *m_inputHandler);
        }

        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }
}

void Engine::shutdown() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Engine::handleResize(const int width, const int height) const {
    if(m_renderer) {
        m_renderer->resize(width, height);
    }
}

void Engine::handleMouse(const double xpos, const double ypos) const {
    if (!m_scene) {
        std::cout << "ERROR: m_scene is null!" << std::endl;
        return;
    }

    if (const ImGuiIO& io = ImGui::GetIO(); io.WantCaptureMouse) {
        m_scene->getCamera().setFirstMouse(true);
        return;
    }

    if (glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        m_scene->getCamera().setFirstMouse(true);
        return;
    }
    m_scene->getCamera().update(xpos, ypos);
}

void Engine::handleScroll(const double xoffset, const double yoffset) const {
    m_scene->getCamera().processScroll(xoffset, yoffset);
}
