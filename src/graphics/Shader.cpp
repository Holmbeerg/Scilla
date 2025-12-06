#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
    : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath) {

    m_shaderID = compileProgram(vertexPath, fragmentPath);
}

unsigned int Shader::compileProgram(const std::string &vPath, const std::string &fPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vPath);
        fShaderFile.open(fPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << vPath << " or " << fPath << std::endl;
        return 0;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        checkCompileErrors(vertex, "VERTEX");
        glDeleteShader(vertex);
        return 0;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        checkCompileErrors(fragment, "FRAGMENT");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    unsigned int programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        checkCompileErrors(programID, "PROGRAM");
        glDeleteProgram(programID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    glDetachShader(programID, vertex);
    glDetachShader(programID, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return programID;
}

bool Shader::reload() {
    const unsigned int newShaderID = compileProgram(m_vertexPath, m_fragmentPath);
    if (newShaderID == 0) {
        std::cerr << "Error: Failed to reload shader program!" << std::endl;
        return false;
    }
    glDeleteProgram(m_shaderID);
    m_shaderID = newShaderID;
    m_uniformLocations.clear(); // clear cached uniform locations
    return true;
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
