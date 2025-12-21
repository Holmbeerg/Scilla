#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();

    explicit Texture(const std::string &imagePath, std::string typeName, bool isColorData = true, bool flipVertically = true);
    explicit Texture(const std::string& imagePath, bool isColorData = true, bool flipVertically = false);

    ~Texture();

    // Delete copy semantics
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    // Move semantics
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    void bindToTextureUnit(unsigned int unit) const;

    void setWrapMode(GLint wrapS, GLint wrapT) const;
    void setFilterMode(GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR) const;

    void loadFromFile(const std::string &imagePath, bool isColorData = true, bool flipVertically = false);

    [[nodiscard]] GLuint getID() const;
    [[nodiscard]] const std::string &getPath() const;
    [[nodiscard]] const std::string &getType() const;

private:
    unsigned int m_textureID;
    std::string m_path;
    std::string m_type;
    int m_width;
    int m_height;
    int m_nrChannels;
};
