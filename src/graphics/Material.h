#pragma once
#include <memory>
#include "Texture.h"
#include "Shader.h"

struct Material {
    // Legacy / Blinn Phong properties
    std::shared_ptr<Texture> diffuseMap;   // "texture_diffuse1"
    std::shared_ptr<Texture> specularMap;  // "texture_specular1"
    float shininess = 32.0f;

    std::shared_ptr<Texture> normalMap;    // "texture_normal1", used by both
    std::shared_ptr<Texture> armMap;       // "texture_arm1" (ambient-roughness-metallic), for PBR. Also called ORM map.

    // Helper to bind this material to a shader
    void bind(const Shader& shader) const {
        // Textures
        if (diffuseMap) {
            diffuseMap->bindToTextureUnit(0);
            shader.setTextureUnit("material.diffuse", 0); // Matching Shader uniform name
        }

        if (normalMap) {
            normalMap->bindToTextureUnit(1);
            shader.setTextureUnit("material.normal", 1);
        }

        if (armMap) {
            armMap->bindToTextureUnit(2);
            shader.setTextureUnit("material.arm", 2);
        }

        if (specularMap) {
            specularMap->bindToTextureUnit(3);
            shader.setTextureUnit("material.specular", 3);
        }

        // Properties
        shader.setFloat("material.shininess", shininess);
    }
};