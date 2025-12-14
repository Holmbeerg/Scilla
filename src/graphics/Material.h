#pragma once
#include <memory>
#include "Texture.h"
#include "Shader.h"

struct Material {
    std::shared_ptr<Texture> diffuseMap;   // "texture_diffuse1"
    std::shared_ptr<Texture> specularMap;  // "texture_specular1"
    std::shared_ptr<Texture> normalMap;    // "texture_normal1"

    // Material properties
    float shininess = 32.0f;

    // Helper to bind this material to a shader
    void bind(const Shader& shader) const {
        // Textures
        if (diffuseMap) {
            diffuseMap->bindToTextureUnit(0);
            shader.setTextureUnit("material.diffuse", 0); // Matching Shader uniform name
        }

        if (specularMap) {
            specularMap->bindToTextureUnit(1);
            shader.setTextureUnit("material.specular", 1);
        } else {
        }

        if (normalMap) {
            normalMap->bindToTextureUnit(2);
            shader.setTextureUnit("material.normal", 2);
        }

        // Properties
        shader.setFloat("material.shininess", shininess);
    }
};