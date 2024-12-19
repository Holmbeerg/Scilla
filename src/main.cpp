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

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "FrameTimer.h"

constexpr unsigned int SCR_WIDTH = 800; // unsigned int = only positive numbers, constexpr = known at compile time
constexpr unsigned int SCR_HEIGHT = 600;

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
                      void const *user_param);

void processInput(GLFWwindow *window);

void setupShaders();

void setupBuffers();

void createAndGenerateTexture();

void interpolationValueControls();

FrameTimer frameTimer;
Camera camera;

VAO vao1, vao2;
VBO vbo1, vbo2;
EBO ebo1;

Texture containerTexture, smileyTexture;

Shader shaderProgram1;
Shader shaderProgram2;

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
    glfwSetKeyCallback(window, key_callback);
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

    shaderProgram2.use();
    // need to specify which texture unit each uniform sampler (sample2D) belongs to
    shaderProgram2.setInt("texture1", 0);
    shaderProgram2.setInt("texture2", 1);

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

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        camera.processInput(window, frameTimer.getDeltaTime());
        frameTimer.update();
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);

        for (GLuint i = 0; i < 10; i++) {

            model = glm::translate(model, cubePositions[i]);

            if (i % 3 == 0) {
                // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            shaderProgram2.setMat4("projection", projection);
            shaderProgram2.setMat4("view", camera.getViewMatrix());
            shaderProgram2.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void createAndGenerateTexture() {
    containerTexture = Texture("textures/container.jpg");
    smileyTexture = Texture("textures/awesomeface.png");

    containerTexture.bindToUnit(0);
    smileyTexture.bindToUnit(1);
}

void setupBuffers() {
    vao1.generate();
    vao2.generate();
    vbo1.generate();
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

    std::cout << "Size of vertex: " << sizeof(vertex) << std::endl;

    const unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    vao1.bindVBO(vbo1, 0, sizeof(vertex));
    // vao2.bindVBO(vbo2, 0, sizeof(vertex));
    vao2.bindVBO(vbo2, 0, 20);
    vao2.bindEBO(ebo1);

    vbo1.setData(vertices, sizeof(vertices));
    vbo2.setData(cubeVertices, sizeof(cubeVertices));
    ebo1.setData(indices, sizeof(indices));

    vao1.enableAttrib(0);
    vao1.enableAttrib(1);
    vao1.enableAttrib(2);

    vao2.enableAttrib(0);
    vao2.enableAttrib(1);
    vao2.enableAttrib(2);

    vao1.setAttribFormat(0, 3, GL_FLOAT, false, offsetof(vertex, position), 0);
    vao1.setAttribFormat(1, 3, GL_FLOAT, false, offsetof(vertex, color), 0);
    vao1.setAttribFormat(2, 2, GL_FLOAT, false, offsetof(vertex, texCoord), 0);

    vao2.setAttribFormat(0, 3, GL_FLOAT, false, offsetof(vertex, position), 0);
    vao2.setAttribFormat(1, 3, GL_FLOAT, false, offsetof(vertex, color), 0);
   // vao2.setAttribFormat(2, 2, GL_FLOAT, false, offsetof(vertex, texCoord), 0);
    vao2.setAttribFormat(2, 2, GL_FLOAT, false, offsetof(cube, texCoord), 0);


    vao2.bind();
}

void setupShaders() {
    const std::string shaderBasePath = "shaders/";
    // left triangle
    shaderProgram1 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader.frag");
    // right triangle
    shaderProgram2 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader2.frag");
}


void toggleWireframeMode() {
    static bool isWireFrame = false; // static to keep value after exiting function
    isWireFrame = !isWireFrame;
    glPolygonMode(GL_FRONT_AND_BACK, isWireFrame ? GL_LINE : GL_FILL);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        toggleWireframeMode();
    }
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



void processInput(GLFWwindow *window) {
    static float currentInterpolationValue = 0.2f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (currentInterpolationValue >= 1.0f) {
        currentInterpolationValue = 1.0f;
    } else if (currentInterpolationValue <= 0.0f) {
        currentInterpolationValue = 0.0f;
    }

    shaderProgram2.setFloat("interpolationValue", currentInterpolationValue);
    static bool isUpPressed = false;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!isUpPressed) {
            currentInterpolationValue += 0.1f;
            isUpPressed = true;
        }
    } else if (isUpPressed && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
        isUpPressed = false;
    }

    static bool isKeyDown = false;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!isKeyDown) {
            currentInterpolationValue -= 0.1f;
            isKeyDown = true;
        }
    } else if (isKeyDown && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
        isKeyDown = false;
    }
}
