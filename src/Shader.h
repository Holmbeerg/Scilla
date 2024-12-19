#pragma once
#include <../glad/glad.h>

#include <string>
#include <glm/glm.hpp>

class Shader {

public:
    // constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    Shader(); // default constructor

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    // use/activate the shader
    void use() const;

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string &name, const glm::mat4 &value) const;

    [[nodiscard]] int getUniformLocation(const std::string &name) const;

    [[nodiscard]] GLuint getId() const;

private:
    GLuint m_shaderID; // Program id (private)

    static void checkCompileErrors(unsigned int shader, const std::string &type);
};