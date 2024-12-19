#include "Texture.h"

#include <stb/stb_image.h>
#include <iostream>

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_nrChannels(0) { }

Texture::Texture(const std::string& imagePath, const bool flipVertically)
    : m_textureID(0), m_width(0), m_height(0), m_nrChannels(0) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    loadFromFile(imagePath, flipVertically);
}

Texture::~Texture() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

// Move constructor, transfer resources in an efficient way
Texture::Texture(Texture&& other) noexcept // "stealing" resources, Texture&& other = rvalue reference, noexcept = no exceptions allowed, better for performance
    : m_textureID(other.m_textureID), 
      m_width(other.m_width), 
      m_height(other.m_height), 
      m_nrChannels(other.m_nrChannels) {
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_nrChannels = 0;
}

// Move assignment
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        // Clean up existing texture
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
        }

        // Move resources
        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_nrChannels = other.m_nrChannels;

        // Reset other
        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_nrChannels = 0;
    }
    return *this;
}

void Texture::bindToUnit(const GLuint unit) const {
    glBindTextureUnit(unit, m_textureID);
}

void Texture::setWrapMode(const GLint wrapS, const GLint wrapT) const {
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, wrapS);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::setFilterMode(const GLint minFilter, const GLint magFilter) const {
    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::loadFromFile(const std::string& imagePath, const bool flipVertically) {
    stbi_set_flip_vertically_on_load(flipVertically);
    
    unsigned char* data = stbi_load(
        imagePath.c_str(), 
        &m_width, 
        &m_height, 
        &m_nrChannels, 
        0
    );

    if (!data) {
        std::cerr << "Failed to load texture: " << imagePath << std::endl;
        return;
    }

    GLenum format = GL_RGB;
    if (m_nrChannels == 4) format = GL_RGBA;
    else if (m_nrChannels == 1) format = GL_RED;

    setWrapMode(GL_REPEAT, GL_REPEAT);
    setFilterMode(GL_LINEAR, GL_LINEAR);

    glTextureStorage2D(m_textureID, 1, GL_RGBA8, m_width, m_height);
    glTextureSubImage2D(
        m_textureID, 0, 0, 0, 
        m_width, m_height, 
        format, GL_UNSIGNED_BYTE, 
        data
    );

    glGenerateTextureMipmap(m_textureID);

    stbi_image_free(data);
}

GLuint Texture::getID() const {
    return m_textureID;
}
