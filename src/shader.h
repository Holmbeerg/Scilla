// include guards, prevent multiple inclusions, preprocessor directives
#ifndef SHADER_H // check if SHADER_H has NOT been defined before
#define SHADER_H // define SHADER_H so its not included again
#include <../glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
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

    [[nodiscard]] GLuint getId() const;

private:
    GLuint m_id; // Program id (private)

    static void checkCompileErrors(unsigned int shader, const std::string &type);
};

#endif //SHADER_H
