#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos; // will be interpolated, per-fragment world position
out vec3 normal; // will be interpolated
out vec2 texCoords; // will be interpolated

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = aNormal * normalMatrix; // transform normals, prevent normals from being skewed from non-uniform scaling
    texCoords = aTexCoord;
}
