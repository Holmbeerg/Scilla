#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = aPos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0); // remove translation from the view matrix, only rotation. The skybox should always be centered around the camera.
    gl_Position = pos.xyww; // set w component to the z component to ensure depth is 1.0. This makes sure the skybox is rendered at the farthest depth.

}