#pragma once

#include <glad.h>
#include <string>

class Texture {
private:
    unsigned int m_textureID;
    int m_width;
    int m_height;
    int m_nrChannels;

public:
    Texture();

    explicit Texture(const std::string& imagePath, bool flipVertically = true);

    ~Texture();

    // prevent compiler from generating copy constructor or copy assignment operator (mostly safety, performance too?)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept; // move constructor
    Texture& operator=(Texture&& other) noexcept; // move assignment

    void bindToUnit(unsigned int unit) const;

    void setWrapMode(GLint wrapS, GLint wrapT) const;
    void setFilterMode(GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR) const;

    void loadFromFile(const std::string& imagePath, bool flipVertically = true);

    [[nodiscard]] GLuint getID() const;
};
