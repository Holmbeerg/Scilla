#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Texture;
class Shader;
class Model;

class AssetManager {
public:
    static AssetManager& get();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::shared_ptr<Texture> loadTexture(
        const std::string& path,
        bool isColorData = true,
        bool flipVertically = true);

    std::shared_ptr<Shader> loadShader(
        const std::string& vertPath,
        const std::string& fragPath);

    std::shared_ptr<Model> loadModel(
        const std::string& path);

    void reloadShaders() const;

    void clear();

private:
    AssetManager() = default; // Private constructor for singleton
    ~AssetManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>  m_shaders;
    std::unordered_map<std::string, std::shared_ptr<Model>>   m_models;
};
