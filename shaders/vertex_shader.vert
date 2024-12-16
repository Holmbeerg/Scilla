#version 460 core
layout (location = 0) in vec3 aPos; // variable aPos has attribute position 0
layout (location = 1 ) in vec3 aColor; // aColor has attribute position 1
layout (location = 2) in vec2 aTexCoord;

out vec3 vertPos;
out vec3 ourColor;
out vec2 texCoord; // interpolation calculation automatically happens after vertex shader and before fragment shader, interpolated across the fragments (pixels)
uniform vec4 translation;
uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0f); // multiply position vector with transformation matrix
    ourColor = aColor;
    vertPos = aPos;
    texCoord = aTexCoord;
}
