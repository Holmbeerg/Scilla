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


const unsigned int SCR_WIDTH = 800; // unsigned int = only positive numbers
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height); // resize window

// function prototype/forward declaration = declaration of function
void processInput(GLFWwindow *window);

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

    // Shaders
    // Vertex shader, run on each vertex
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // shader object, referenced by ID
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // attach shader source code to shader object
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATON_FAILED\n" << infoLog << "\n";
    }

    unsigned int shaderProgram = glCreateProgram(); // object, final linked version of multiple shaders combined
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // link all attached shaders in one final shader program object

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    // no longer need shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // three vertices (x, y, z)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // Vertex Buffer Objects, store vertices in the GPU's memory, can send large batches of data to the GPU at once
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    /**
     * Vertex Array Object
     *To use a VAO all you have to do is bind the VAO using glBindVertexArray. From that point on we
     *should bind/configure the corresponding VBO(s) and attribute pointer(s) and then unbind the VAO
     *for later use.

     *VAO stores our vertex attribute configuration and which VBO to use. Usually when you have
     *multiple objects you want to draw, you first generate/configure all the VAOs (and thus the required
     *VBO and attribute pointers) and store those for later use. The moment we want to draw one of our objects,
     *we take the corresponding VAO, bind it, then draw the object and unbind the VAO again.
     */

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // bind buffer to GL_ARRAY_BUFFER, any buffer calls made on GL_ARRAY_BUFFER will be used to configure currently bound buffer (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy user-defined data into the currently bound buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // index: attribute location ( layout(location = 0) )
    // size: number of components for vertex attribute, vec3 so 3 components
    // stride: each vertex has 3 components that are floats, how many bytes between consecutive vertex attributes
    // (void*)0): offset where vertex attribute starts in buffer, data starts from beginning here
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0); // disabled by default
    glBindBuffer(GL_ARRAY_BUFFER, 0); // registered in glVertexAttribPointer, can safely unbind

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT);
        // uses value from glClearColor, state-using function. state of OpenGL = OpenGL context
        glUseProgram(shaderProgram);
        // Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwPollEvents(); // checks for keyboard input, mouse movement events etc
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
