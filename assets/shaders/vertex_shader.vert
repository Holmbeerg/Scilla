// The vertex shader runs once for each vertex. Transforms vertex data from Object Space to Clip Space.
// At the end of the vertex shader step, perspective division is automatically performed and
// transforms the 4D Clip Space coordinates into 3D Normalized Device Coordinates (NDC).
// These coordinates are then mapped to screen coordinates using the settings of glViewport and transformed into fragments
// as input for the Fragment Shader.

#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

// Uniform buffer object (UBO) for camera data, shared between multiple shaders
layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

// "out" variables are passed to the Fragment Shader.
// The GPU automatically interpolates these values across the triangle's surface.
out vec3 fragPos;
out vec3 normal;
out vec2 texCoords; // the textures uv coordinates
out mat3 TBN; // Tangent, Bitangent, Normal matrix to transform from tangent space to world space

// Uniforms are global variables within a shader that remain constant for all processed vertices for a single draw call.
uniform mat4 model;       // Transforms Object Space -> World Space
uniform mat3 normalMatrix;

void main() {
    fragPos = vec3(model * vec4(aPos, 1.0)); // calculate fragment position in world space, for lighting calculations
    texCoords = aTexCoord;

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N); // Gram-Schmidt process
    vec3 B = normalize(cross(N, T)); // Calculate the bitangent

    TBN = mat3(T, B, N); // convert from tangent space to world space

    normal = N;
    gl_Position = projection * view * vec4(fragPos, 1.0f); // gl_Position is a special variable that holds the final position of the vertex in Clip Space
}
