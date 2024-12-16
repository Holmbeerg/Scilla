#include <cmath>
#include <complex>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <stb/stb_image.h>
#include "shader.h"

constexpr unsigned int SCR_WIDTH = 1000; // unsigned int = only positive numbers, constexpr = known at compile time
constexpr unsigned int SCR_HEIGHT = 600;

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void processInput(GLFWwindow *window);

void setupShaders();
void setupBuffers();
void createAndGenerateTexture();
void interpolationValueControls();

Shader shaderProgram1; // added default constructor to shader class
Shader shaderProgram2;
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
    createAndGenerateTexture();

    shaderProgram2.use();
    shaderProgram2.setInt("texture1", 0); // need to specify which texture unit each uniform sampler (sample2D) belongs to
    shaderProgram2.setInt("texture2", 1);

    // https://stackoverflow.com/questions/59222806/how-does-glm-handle-translation
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f); // define a vector
    glm::mat4 inputMatrix = glm::mat4(1.0f); // initialize a matrix, IDENTITY MATRIX with 1.0f
    inputMatrix = glm::scale(inputMatrix, glm::vec3(0.5f, 0.5f, 0.5f)); // scale the container by 0.5 on each axis
    inputMatrix = glm::rotate(inputMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate 90 degrees around z-axis
    // GLuint transformLocation = glGetUniformLocation(shaderProgram2.getId(), "transform");
    //glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(inputMatrix));

    // 1. We give as input a matrix (in this case IDENTITY MATRIX)
    // 2. A translation matrix is formed based on given translation vector. Travel a distance of 1 along x-axis, 1 along y-axis and 0 along z-axis.
    // 3. Multiply input matrix  with formed translation matrix and return. Return resulting matrix.
    // inputMatrix = glm::translate(inputMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

    vec = inputMatrix * vec; // transformed vector
    std::cout << vec.x << vec.y << vec.z << std::endl;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

        glBindVertexArray(VAO[1]);
        //glDrawArrays(GL_TRIANGLES, 0, 4); // first: starting index of currently bound VAO, count of vertices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // 6 indices used!

        //Here we first rotate the container around the origin (0,0,0) and once it's rotated, we translate its
        //rotated version to the bottom-right corner of the screen. Remember that the actual transformation order
        //should be read in reverse: even though in code we first translate and then later rotate, the actual transformations
        //first apply a rotation and then a translation.Here we first rotate the container around the origin (0,0,0) and
        //once it's rotated, we translate its rotated version to the bottom-right corner of the screen. Remember that the
        //actual transformation order should be read in reverse: even though in code we first translate and then later rotate
        //, the actual transformations first apply a rotation and then a translation.
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(2.0f, 0.5f, 1.0f));
        GLuint transformLocation = glGetUniformLocation(shaderProgram2.getId(), "transform");
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
        
        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void createAndGenerateTexture() {
    GLint width, height, nrChannels;
    // Takes image location and fills parameters with the images width, height and number of color channels (RGB = 3, RGBA = 4), height and width needed to generate textures
    stbi_set_flip_vertically_on_load(true); // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image, but images usually have 0.0 at the top of the y-axis
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
    unsigned char *data2 = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);

    if (!data || !data2) {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

    // Textures
    unsigned int texture1, texture2; // Texture object
    // Texture 1
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0); // set active Texture Unit to GL_TEXTURE0
    glBindTexture(GL_TEXTURE_2D, texture1); // Bind the texture object to the texture target GL_TEXTURE_2D
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data); // free data to free up memory

    // Texture 2
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1); // set active Texture Unit to GL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify texture parameters, If we load a PNG image we need to specify that image contains ALPHA channel (transparency) with GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);
    glGenerateMipmap(GL_TEXTURE_2D); // generate all required mipmaps for currently bound texture
}

void setupBuffers() {
    // Three vertices (x, y, z). Vertices = multiple 3D points, vertex = single point
    // Position, color, texture are vertex attributes
    constexpr float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
   };

    constexpr float vertices2[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
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
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); // bind VBO buffer object to bind target GL_ARRAY_BUFFER, this VBO is now the active buffer OpenGL will use
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // add vertices data to bind target
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0); // stride 8*float=24bytes
    glEnableVertexAttribArray(0); // enable once for attribute
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float))); // color attribute starts after position data with 3 components
    glEnableVertexAttribArray(1);
    // texture position
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // SETTING UP VAO[1] and VBO[1]
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // bind VBO buffer to GL_ARRAY_BUFFER, any buffer calls made on GL_ARRAY_BUFFER will be used to configure currently bound buffer (VBO)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0); // new VAO object = need to bind again!
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture position
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind ebo to VAO[1]
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copy user-defined data into the currently bound buffer

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // registered in glVertexAttribPointer, can safely unbind
}

void setupShaders() {
    const std::string shaderBasePath = "/home/holmberg/development/CLionProjects/OpenGL/shaders/";
    shaderProgram1 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader.frag"); // left triangle
    shaderProgram2 = Shader(shaderBasePath + "vertex_shader.vert", shaderBasePath + "fragment_shader2.frag"); // right triangle
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


float currentInterpolationValue = 0.2f;
void processInput(GLFWwindow *window) {
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

