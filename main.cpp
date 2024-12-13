#include <cmath>
#include <complex>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shaders/shader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

constexpr unsigned int SCR_WIDTH = 1000; // unsigned int = only positive numbers, constexpr = known at compile time
constexpr unsigned int SCR_HEIGHT = 600;

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void processInput(GLFWwindow *window);

void setupShaders();
void setupBuffers();

Shader ourShader1; // added default constructor to shader class
Shader ourShader2;
GLuint VBO[2], VAO[2], EBO;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    setupShaders();
    setupBuffers();
    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT);
        // uses value from glClearColor, state-using function. state of OpenGL = OpenGL context
        const float timeValue = glfwGetTime();
        const float greenValue = std::sin(timeValue) / 2.0f + 0.5f;
        const float offset = 0.5f + std::sin(timeValue) / 2;

        glm::vec4 color(0.0f, greenValue, 0.0f, 1.0f);
        glm::vec4 translation(offset, 0.0f, 0.0f, 0.0f);

        ourShader1.use();
        //ourShader1.setVec4("ourColor", color);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // first: starting index of currently bound VAO, only vertices

        ourShader2.use();
        ourShader2.setVec4("ourColor", color);
        ourShader2.setVec4("translation", translation);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // const int vertexColorLocation = glGetUniformLocation(shaderProgram2, "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // set a uniform value of currently active shader program



        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

        // we have 6 indices, does indexed drawing and renders vertices from VBO in the order specified in EBO
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void setupBuffers() {
    // three vertices (x, y, z)
    constexpr float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f // bottom left
    };

    constexpr float vertices2[] = {
        -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top left (blue)
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left (blue)
        -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top right (blue)
    };

    const unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // Vertex Buffer Objects, store vertices in the GPU's memory, can send large batches of data to the GPU at once
    // unsigned int VBO, VAO, EBO;
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO); // stored in array
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // SETTING UP VAO[0] and VBO[0]
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); // bind VBO buffer to GL_ARRAY_BUFFER, any buffer calls made on GL_ARRAY_BUFFER will be used to configure currently bound buffer (VBO)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0); // stride 6*float=24bytes
    glEnableVertexAttribArray(0); // enable once for attribute
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float))); // color attribute starts after position data with 3 components
    glEnableVertexAttribArray(1);

    // SETTING UP VAO[1] and VBO[1]
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // bind VBO buffer to GL_ARRAY_BUFFER, any buffer calls made on GL_ARRAY_BUFFER will be used to configure currently bound buffer (VBO)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0); // new VAO object = need to bind again!
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copy user-defined data into the currently bound buffer

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // registered in glVertexAttribPointer, can safely unbind
}

void setupShaders() {
    const std::string shaderBasePath = "/home/holmberg/development/CLionProjects/OpenGL/shaders/";
    ourShader1 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader.frag");
    ourShader2 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader2.frag");
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

