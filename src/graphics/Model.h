#pragma once
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Mesh.h"

class Model {
public:
    explicit Model(const std::string &path, const bool gamma = false) : m_gammaCorrection(gamma) {
        loadModel(path);
    }

    void render(const Shader &shader) const;

private:
    std::vector<Mesh> m_meshes;

    // Texture cache. Stores textures already loaded from disk.
    // shared_ptr is used so multiple meshes can reference the same texture.
    // The texture is automatically deleted when no mesh uses it anymore.
    std::vector<std::shared_ptr<Texture>> m_textures_loaded;
    std::string m_directory;
    bool m_gammaCorrection;

    void loadModel(const std::string &path);

    void processNode(const aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) const;

    std::shared_ptr<Texture> loadMaterialTexture(const aiMaterial *mat, aiTextureType type, bool isSRGB) const;
};
