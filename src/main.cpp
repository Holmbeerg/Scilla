#include <cmath>
#include <complex>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <utility>

#include "camera/Camera.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Cube.h"

#include "graphics/VAO.h"
#include "graphics/VBO.h"
#include "graphics/EBO.h"
#include "graphics/Skybox.h"
#include "utils/FrameTimer.h"
#include "input/InputHandler.h"

constexpr unsigned int SCR_WIDTH = 800; // unsigned int = only positive numbers, constexpr = known at compile time
constexpr unsigned int SCR_HEIGHT = 600;

std::vector<std::string> faces = {
    "assets/textures/skybox/right.jpg",
    "assets/textures/skybox/left.jpg",
    "assets/textures/skybox/top.jpg",
    "assets/textures/skybox/bottom.jpg",
    "assets/textures/skybox/front.jpg",
    "assets/textures/skybox/back.jpg"
};

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param);

void processInput(GLFWwindow *window);

void setupShaders();

void setupBuffers();

void createAndGenerateTexture();

void interpolationValueControls();

void setupLightCube();

void renderCube(const VAO &vao);

FrameTimer frameTimer;
Camera camera;

VAO lightSourceVao;
VBO vbo2;
EBO ebo1;

Shader objectShader;
Shader lightSourceShader;
Shader skyboxShader;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_FRAMEBUFFER_SRGB); // enable sRGB gamma correction
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDebugMessageCallback(message_callback, nullptr);

    Skybox skybox(faces);

    setupShaders();
    setupLightCube();

    const Model backpack("assets/models/backpack/backpack.obj", true);

    InputHandler input_handler(window);

    const std::vector<std::string> vertexUniforms = {
        "projection",
        "view",
        "model",
        "normalMatrix"
    };

    const std::vector<std::string> objectLightUniforms = {
        "viewPos",
        "material",
        "light",
    };

    constexpr auto lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    constexpr Shader::Light light{
        lightPos,
        glm::vec3(0.05, 0.05f, 0.05f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    constexpr Shader::Material material{
        64.0f
    };

    objectShader.initializeUniformLocations(vertexUniforms);
    objectShader.initializeUniformLocations(objectLightUniforms);
    lightSourceShader.initializeUniformLocations(vertexUniforms);

    objectShader.use();
    objectShader.setLightProperties(light);
    objectShader.setMaterialProperties(material);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        camera.processInput(window, frameTimer.getDeltaTime());
        frameTimer.update();
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()),
                                                SCR_WIDTH / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f); // anything outside this range will not be rendered
        glm::mat4 view = camera.getViewMatrix();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw backpack
        objectShader.use();
        objectShader.setViewProjection(view, projection);
        objectShader.setVec3("viewPos", camera.getCameraPos()); // camera position is needed for specular lighting
        objectShader.setBool("enableNormalMapping", input_handler.isNormalMappingEnabled());

        auto objectModel = glm::mat4(1.0f); // identity matrix
        objectModel = glm::translate(objectModel, glm::vec3(0.0f, 0.0f, -3.0f));
        objectModel = glm::scale(objectModel, glm::vec3(1.0f));

        auto normalMatrix = glm::mat3(transpose(inverse(objectModel)));
        objectShader.setMat4("model", objectModel);
        objectShader.setMat3("normalMatrix", normalMatrix);

        backpack.render(objectShader);

        // Draw light source cube
        lightSourceShader.use();
        lightSourceShader.setViewProjection(view, projection);

        auto lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.5f));

        normalMatrix = glm::mat3(transpose(inverse(lightModel)));
        lightSourceShader.setMat3("normalMatrix", normalMatrix);
        lightSourceShader.setMat4("model", lightModel);

        renderCube(lightSourceVao);

        skybox.render(skyboxShader, view, projection); // draw skybox last to prevent overdraw

        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);

        if (input_handler.shouldReloadShaders()) {
            std::cout << "Reloading shaders..." << std::endl;
            if (objectShader.reload()) {
                objectShader.initializeUniformLocations(vertexUniforms);
                objectShader.initializeUniformLocations(objectLightUniforms);

                objectShader.use();
                objectShader.setLightProperties(light);
                objectShader.setMaterialProperties(material);
                objectShader.setBool("enableNormalMapping", input_handler.isNormalMappingEnabled());
            }

            if (lightSourceShader.reload()) {
                lightSourceShader.initializeUniformLocations(vertexUniforms);
            }
            input_handler.resetReloadFlag();
        }
    }
    glfwTerminate();
    return 0;
}

void setupLightCube() {
    lightSourceVao.generate();
    vbo2.generate();
    ebo1.generate();

    struct cube {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    lightSourceVao.bindVBO(vbo2, 0, sizeof(cube));
    vbo2.setData(Primitives::cubeVertices, sizeof(Primitives::cubeVertices));

    lightSourceVao.enableAttrib(0);
    lightSourceVao.setAttribFormat(0, 3, GL_FLOAT, false, offsetof(cube, position), 0);
    lightSourceVao.bind();
}

void renderCube(const VAO &vao) {
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void setupShaders() {
    const std::string shaderBasePath = "assets/shaders/";
    objectShader = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader.frag");
    lightSourceShader = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "lightSource.frag");
    skyboxShader = Shader(shaderBasePath + "skybox.vert", shaderBasePath + "skybox.frag");
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        camera.setFirstMouse(true);
        return;
    }
    camera.update(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.processScroll(xoffset, yoffset);
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
    auto const src_str = [source]() {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        }
    }();

    auto const type_str = [type]() {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        }
    }();
    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}
