#include "Skybox.h"

#include <iostream>
#include <ostream>

#include "stb_image.h"

static constexpr float skyboxVertices[] = {
    -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
};

Skybox::Skybox(const std::vector<std::string> &faces)
    : m_VAO(0), m_VBO(0) {
    glCreateVertexArrays(1, &m_VAO);

    glCreateBuffers(1, &m_VBO);
    glNamedBufferStorage(m_VBO, sizeof(skyboxVertices), skyboxVertices, 0);

    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, 3 * sizeof(float));

    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO, 0, 0);

    m_cubemapTexture = loadCubemap(faces);
}

void Skybox::render(const Shader &shader) const {
    glDepthFunc(GL_LEQUAL);  // Change depth function to LEQUAL because the skybox is exactly at depth 1.0
    shader.use();

    glBindTextureUnit(0, m_cubemapTexture);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS); // set depth function back to default
}

unsigned int Skybox::loadCubemap(const std::vector<std::string> &faces) {
    unsigned int textureID;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(false); // Do not flip for cubemaps!

    unsigned char* data = stbi_load(faces[0].c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Cubemap tex failed to load at path: " << faces[0] << std::endl;
        return textureID;
    }

    glTextureStorage2D(textureID, 1, GL_RGB8, width, height);

    glTextureSubImage3D(textureID, 0, 0, 0, 0, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    for (unsigned int i = 1; i < faces.size(); i++) {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTextureSubImage3D(textureID, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}