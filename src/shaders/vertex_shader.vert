#version 460 core
layout (location = 0) in vec3 aPos; // variable aPos has attribute position 0
// layout (location = 1 ) in vec3 aColor; // aColor has attribute position 1
layout (location = 2) in vec2 aTexCoord;

out vec3 vertPos;
out vec3 ourColor;
out vec2 texCoord; // interpolation calculation automatically happens after vertex shader and before fragment shader, interpolated across the fragments (pixels)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // ourColor = aColor;
    vertPos = aPos;
    texCoord = aTexCoord;
}
