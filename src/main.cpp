#include <cmath>
#include <complex>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <filesystem>

#include "camera/Camera.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/VAO.h"
#include "graphics/VBO.h"
#include "graphics/EBO.h"
#include "utils/FrameTimer.h"
#include "input/InputHandler.h"

constexpr unsigned int SCR_WIDTH = 800; // unsigned int = only positive numbers, constexpr = known at compile time
constexpr unsigned int SCR_HEIGHT = 600;

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param);

void processInput(GLFWwindow *window);

void setupShaders();

void setupBuffers();

void createAndGenerateTexture();

void interpolationValueControls();

void drawCube(const VAO &vao);

FrameTimer frameTimer;
Camera camera;

VAO lightSourceVao, objectVao;
VBO vbo1, vbo2;
EBO ebo1;

Texture containerTexture, hedgehogTexture;

Shader objectShader;
Shader lightSourceShader;

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
    glDebugMessageCallback(message_callback, nullptr);
    setupShaders();
    setupBuffers();
    createAndGenerateTexture();

    InputHandler input_handler(window);

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 toyColor = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 result = lightColor * toyColor;

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    const std::vector<std::string> locationUniforms = {
        "projection",
        "view",
        "model",
    };

    const std::vector<std::string> objectLight = {
    "lightColor",
    "objectColor",
    };

    objectShader.initializeUniformLocations(locationUniforms);
    objectShader.initializeUniformLocations(objectLight);
    lightSourceShader.initializeUniformLocations(locationUniforms);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        camera.processInput(window, frameTimer.getDeltaTime());
        frameTimer.update();
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Object shader
        objectShader.use();
        objectShader.setVec3("objectColor", toyColor);
        objectShader.setVec3("lightColor", lightColor);
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);

        glm::mat4 objectModel = glm::translate(glm::mat4(1.0f), cubePositions[0]);
        objectShader.setMat4("model", objectModel);

        drawCube(objectVao);

        // Light source shader
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);

        glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), cubePositions[7]);
        lightModel = glm::scale(lightModel, glm::vec3(0.5f));
        lightSourceShader.setMat4("model", lightModel);

        drawCube(lightSourceVao);

        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void drawCube(const VAO &vao) {
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void createAndGenerateTexture() {

}

void setupBuffers() {
    objectVao.generate();
    lightSourceVao.generate();
    vbo2.generate();
    ebo1.generate();

    struct vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    struct cube {
        glm::vec3 position;
        glm::vec2 texCoord;
    };

    std::cout << offsetof(vertex, texCoord) << std::endl;

    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    constexpr float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    };

    const unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    objectVao.bindVBO(vbo2, 0, 20);
    lightSourceVao.bindVBO(vbo2, 0, 20);
    objectVao.bindEBO(ebo1);

    vbo2.setData(cubeVertices, sizeof(cubeVertices));
    ebo1.setData(indices, sizeof(indices));

    objectVao.enableAttrib(0);
    objectVao.setAttribFormat(0, 3, GL_FLOAT, false, offsetof(cube, position), 0);

    lightSourceVao.enableAttrib(0);
    lightSourceVao.setAttribFormat(0, 3, GL_FLOAT, false, offsetof(cube, position), 0);
}

void setupShaders() {
    const std::string shaderBasePath = "shaders/";
    objectShader = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader.frag");
    lightSourceShader = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "lightSource.frag");
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.update(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScroll(xoffset, yoffset);
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param) {
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
