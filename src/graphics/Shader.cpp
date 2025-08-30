#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    // Load vertex and fragment shader code
    std::string vertexCode;
    std::string fragmentCode;

    try {
        std::ifstream fShaderFile;
        std::ifstream vShaderFile;

        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

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
    m_shaderID = glCreateProgram(); // set m_id instance variable
    glAttachShader(m_shaderID, vertex);
    glAttachShader(m_shaderID, fragment);
    glLinkProgram(m_shaderID);
    glDetachShader(m_shaderID, vertex); // Detach after linking
    glDetachShader(m_shaderID, fragment);
    checkCompileErrors(m_shaderID, "PROGRAM");

    // Delete shaders after linking (they are no longer needed)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader() : m_shaderID(0) { }

void Shader::use() const {
    if (m_shaderID == 0) {
        std::cerr << "Error: Shader not initialized!" << std::endl;
        return;
    }
    glUseProgram(m_shaderID);
}

void Shader::setBool(const std::string &name, const bool value) const {
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, const int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, const float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::setLightProperties(const Light &light) const {
    setVec3("light.position", light.position);
    setVec3("light.ambient", light.ambient); // needs to be lowercase
    setVec3("light.diffuse", light.diffuse);
    setVec3("light.specular", light.specular);
}

void Shader::setMaterialProperties(const Material &material) const {
    setFloat("material.shininess", material.shininess);
}

void Shader::setViewProjection(const glm::mat4& view, const glm::mat4& projection) const {
    setMat4("projection", projection);
    setMat4("view", view);
}


GLuint Shader::getId() const {
    return m_shaderID;
}

int Shader::getUniformLocation(const std::string &name) const {
    if (const auto it = m_uniformLocations.find(name); it != m_uniformLocations.end()) {
        return it->second;
    }
    // fallback
    const GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    m_uniformLocations[name] = location;
    return location;
}

void Shader::initializeUniformLocations(const std::vector<std::string> &uniformNames) const { // caching uniform locations
        for (const auto &name : uniformNames) {
            m_uniformLocations[name] = glGetUniformLocation(m_shaderID, name.c_str());
        }
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
