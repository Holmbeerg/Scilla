#include "Texture.h"

#include <cmath>
#include <stb/stb_image.h>
#include <iostream>

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_nrChannels(0) {
}

Texture::Texture(const std::string &imagePath, std::string typeName, const bool isColorData, const bool flipVertically)
    : m_textureID(0),
      m_path(imagePath),
      m_type(std::move(typeName)),
      m_width(0),
      m_height(0),
      m_nrChannels(0) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    loadFromFile(imagePath, isColorData, flipVertically);
}

Texture::~Texture() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

// Move constructor, transfer resources in an efficient way
// "stealing" resources, Texture&& other = rvalue reference, noexcept = no exceptions allowed, better for performance
Texture::Texture(Texture &&other) noexcept
    : m_textureID(other.m_textureID),
      m_path(std::move(other.m_path)),
      m_type(std::move(other.m_type)),
      m_width(other.m_width),
      m_height(other.m_height),
      m_nrChannels(other.m_nrChannels) {
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_nrChannels = 0;
}

// Move assignment
Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        // Clean up existing texture
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
        }

        // Move resources
        m_textureID = other.m_textureID;
        m_path = std::move(other.m_path);
        m_type = std::move(other.m_type);
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

void Texture::bindToTextureUnit(const GLuint unit) const {
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

void Texture::loadFromFile(const std::string &imagePath, const bool isColorData, const bool flipVertically) {
    stbi_set_flip_vertically_on_load(flipVertically);

    unsigned char *data = stbi_load(
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

    GLenum dataFormat = 0;
    GLenum internalFormat = 0;

    if (m_nrChannels == 4) {
        dataFormat = GL_RGBA;
        internalFormat = isColorData ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        // glTextureStorage2D requires us to be precise about internal format, unlike glTexImage2D
    } else if (m_nrChannels == 3) {
        dataFormat = GL_RGB;
        internalFormat = isColorData ? GL_SRGB8 : GL_RGB8;
    } else if (m_nrChannels == 1) {
        dataFormat = GL_RED;
        internalFormat = GL_R8;
    } else {
        std::cerr << "Unsupported number of channels: " << m_nrChannels << " in texture: " << imagePath << std::endl;
        stbi_image_free(data);
        return;
    }

    setWrapMode(GL_REPEAT, GL_REPEAT);
    setFilterMode(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    const GLsizei levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(m_width, m_height))));

    glTextureStorage2D(m_textureID, levels, internalFormat, m_width, m_height);

    // is this needed? pixel row alignment, default is 4
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTextureSubImage2D(
        m_textureID,
        0, 0, 0,
        m_width, m_height,
        dataFormat,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateTextureMipmap(m_textureID);

    stbi_image_free(data);
}

GLuint Texture::getID() const {
    return m_textureID;
}

const std::string &Texture::getPath() const {
    return m_path;
}

const std::string &Texture::getType() const {
    return m_type;
}
