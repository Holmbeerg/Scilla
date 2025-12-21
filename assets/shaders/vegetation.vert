#version 460 core

// Standard Mesh Attributes (Binding Point 0)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// Instancing Attributes (Binding Point 1)
// A mat4 takes up 4 attribute slots (locations 3, 4, 5, 6)
layout (location = 3) in mat4 aInstanceMatrix;
layout (location = 7) in vec3 aTangent;
layout (location = 8) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;

// Global Uniforms
layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

void main() {
    vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoords = aTexCoords;

    // Calculate normal matrix (for non-uniform scaling)
    mat3 normalMatrix = mat3(aInstanceMatrix);

    // Transform to world space
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);

    // Gram-Schmidt
    T = normalize(T - dot(T, N) * N);

    // Recalculate B to ensure perfect orthogonality
    B = cross(N, T);

    TBN = mat3(T, B, N);

    Normal = N;

    gl_Position = projection * view * worldPos;
}