#include "Model.h"
#include <assimp/postprocess.h>
#include <iostream>
#include <src/core/AssetManager.h>

void Model::render(const Shader &shader) const {
    for (const auto &mesh : m_meshes) {
        mesh.render(shader);
    }
}

void Model::loadModel(const std::string &path) {
    Assimp::Importer importer;
    // https://the-asset-importer-lib-documentation.readthedocs.io/en/latest/usage/postprocessing.html
    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
              aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) const {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents) {
            vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
        } else {
            vertex.Tangent = glm::vec3(0.0f);
        }
        vertices.push_back(vertex);
    }

    // Process Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process Material
    Material material;

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];

        material.diffuseMap  = loadMaterialTexture(aiMat, aiTextureType_DIFFUSE, true);
        material.specularMap = loadMaterialTexture(aiMat, aiTextureType_SPECULAR, false);

        // Assimp often puts Normal maps in aiTextureType_HEIGHT
        material.normalMap = loadMaterialTexture(aiMat, aiTextureType_HEIGHT, false);
        if (!material.normalMap) {
             // Fallback: Check standard normals if height was empty
             material.normalMap = loadMaterialTexture(aiMat, aiTextureType_NORMALS, false);
        }

        // Load Shininess
        if (float shininess; aiMat->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
             material.shininess = shininess;
        }
    }

    return {vertices, indices, material};
}

std::shared_ptr<Texture> Model::loadMaterialTexture(const aiMaterial *mat, const aiTextureType type, const bool isSRGB) const {
    if (mat->GetTextureCount(type) == 0) return nullptr;

    aiString str;
    mat->GetTexture(type, 0, &str); // Get only the first texture of this type

    const std::string fullPath = m_directory + "/" + str.C_Str();

    return AssetManager::get().loadTexture(fullPath, isSRGB, true);
}
