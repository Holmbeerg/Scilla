#version 460 core
layout (location = 0) in vec3 aPos; // variable aPos has attribute position 0
layout (location = 1 ) in vec3 aColor; // aColor has attribute position 1

out vec3 ourColor;
uniform vec4 translation;

void main()
{
    gl_Position = vec4(aPos.x + translation.x, aPos.y + translation.y, aPos.z + translation.z, 1.0); // can pass vec3 into vec4
    ourColor = aColor;
}
