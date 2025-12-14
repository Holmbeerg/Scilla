// The AssetManager is responsible for loading and caching assets like textures, shaders, and models.

#include "AssetManager.h"

#include "graphics/Texture.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"

AssetManager &AssetManager::get() {
    static AssetManager instance;
    return instance;
}

std::shared_ptr<Texture> AssetManager::loadTexture(
    const std::string &path,
    bool isColorData,
    bool flipVertically) {
    if (m_textures.contains(path))
        return m_textures[path];

    auto texture = std::make_shared<Texture>(
        path, isColorData, flipVertically);

    m_textures[path] = texture;
    return texture;
}

std::shared_ptr<Shader> AssetManager::loadShader(
    const std::string &vertPath,
    const std::string &fragPath) {
    const std::string key = vertPath + "|" + fragPath;

    if (m_shaders.contains(key))
        return m_shaders[key];

    auto shader = std::make_shared<Shader>(vertPath, fragPath);
    m_shaders[key] = shader;
    return shader;
}

std::shared_ptr<Model> AssetManager::loadModel(
    const std::string &path) {
    if (m_models.contains(path))
        return m_models[path];

    auto model = std::make_shared<Model>(path);
    m_models[path] = model;
    return model;
}

void AssetManager::reloadShaders() const {
    for (const auto& [key, shader] : m_shaders) {
        shader->reload();
    }
}

void AssetManager::clear() {
    m_models.clear();
    m_shaders.clear();
    m_textures.clear();
}