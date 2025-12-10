#pragma once
#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

class Shader {

public:
    struct Light {
        glm::vec3 position;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct Material {
        float shininess;
    };

    // constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    Shader(); // default constructor

    bool reload();

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    // use/activate the shader
    void use() const;

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string &name, const glm::mat4 &value) const;

    void setMat3(const std::string &name, const glm::mat3 &value) const;

    void setLightProperties(const Light &light) const;

    void setMaterialProperties(const Material &material) const;

    void setViewProjection(const glm::mat4& view, const glm::mat4& projection) const;

    void setTextureUnit(const std::string &name, int unit) const;

    [[nodiscard]] GLuint getID() const;

    void initializeUniformLocations(const std::vector<std::string> &uniformNames) const;

private:
    GLuint m_shaderID;
    std::string m_vertexPath;
    std::string m_fragmentPath;

    mutable std::unordered_map<std::string, GLint> m_uniformLocations;

    [[nodiscard]] int getUniformLocation(const std::string &name) const;

    static unsigned int compileProgram(const std::string& vPath, const std::string& fPath);
    static void checkCompileErrors(unsigned int shader, const std::string &type);
};