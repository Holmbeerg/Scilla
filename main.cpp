#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// declares an input attribute called aPos which holds the 3D coordinates (position) of the vertex.
// the location = 0 part specifies that this attribute will be bound to location 0.
const char *vertexShaderSource = "#version 460 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

// only requires one output variable, vec4 defines final color output we should calculate ourselves
// red, green, blue, alpha (opacity) where 1 = opaque
const char *fragmentShaderSource = "#version 460 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

const char *fragmentShaderSource2 = "#version 460 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
        "}\0";

const unsigned int SCR_WIDTH = 1000; // unsigned int = only positive numbers
const unsigned int SCR_HEIGHT = 600;

// function prototype/forward declaration = declaration of function
void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window
void processInput(GLFWwindow *window);

void setupShaders();
void setupBuffers();

GLuint VBO[2], VAO[2], EBO;
GLuint shaderProgramOrange, shaderProgramYellow;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
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

        glUseProgram(shaderProgramOrange);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // first: starting index of currently bound VAO, only vertices

        glUseProgram(shaderProgramYellow);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
    };

    float vertices2[] = {
        -0.5f, 0.0f, 0.0f, // top left
        -0.5f, -0.5f, 0.0f, // bottom left
        -1.0f, -0.5f, 0.0f, // top right
    };
    unsigned int indices[] = {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0); // enable once for attribute

    // SETTING UP VAO[1] and VBO[1]
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // bind VBO buffer to GL_ARRAY_BUFFER, any buffer calls made on GL_ARRAY_BUFFER will be used to configure currently bound buffer (VBO)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0); // new VAO object = need to bind again!

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copy user-defined data into the currently bound buffer

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // registered in glVertexAttribPointer, can safely unbind
}

void setupShaders() {
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // shader object, referenced by ID
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // attach shader source code to shader object
    glCompileShader(vertexShader);

    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    // Fragment shader 1
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Fragment shader 2
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, nullptr);
    glCompileShader(fragmentShader2);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATON_FAILED\n" << infoLog << "\n";
    }

    // shader 1
    shaderProgramOrange = glCreateProgram(); // object, final linked version of multiple shaders combined
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShader);
    glLinkProgram(shaderProgramOrange); // link all attached shaders in one final shader program object

    // shader 2
    shaderProgramYellow = glCreateProgram();
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShader2);
    glLinkProgram(shaderProgramYellow);


    glGetProgramiv(shaderProgramOrange, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramOrange, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    // no longer need shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
    // glUseProgram(shaderProgram);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
