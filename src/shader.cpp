#include "shader.h"
#include <iostream>
#include <fstream>
#include <sstream>


Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    // Load vertex and fragment shader code
    std::string vertexCode;
    std::string fragmentCode;

    try {
        std::ifstream fShaderFile;
        std::ifstream vShaderFile;

        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile vertex shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Compile fragment shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Link shaders into a program
    m_id = glCreateProgram(); // set m_id instance variable
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    glDetachShader(m_id, vertex); // Detach after linking
    glDetachShader(m_id, fragment);
    checkCompileErrors(m_id, "PROGRAM");

    // Delete shaders after linking (they are no longer needed)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader() : m_id(0) { }

void Shader::use() const {
    if (m_id == 0) {
        std::cerr << "Error: Shader not initialized!" << std::endl;
        return;
    }
    glUseProgram(m_id);
}


void Shader::setBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int) value); // 1 int
}

void Shader::setInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value); // 1 float
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z, value.w);
}

GLuint Shader::getId() const {
    return m_id;
}


void Shader::checkCompileErrors(const unsigned int shader, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    }
}
