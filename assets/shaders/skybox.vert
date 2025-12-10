#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

// Uniform buffer object (UBO) for camera data, shared between multiple shaders
//Instead of uploading the view and projection matrices individually for every single shader,
// we upload them once to a UBO, and all shaders read from that same memory.
// binding point 0 is used for camera data
layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

void main()
{
    localPos = aPos; //
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0); // remove translation from the view matrix, only rotation. The skybox should always be centered around the camera.
    gl_Position = pos.xyww; // set w component to the z component to ensure depth is 1.0. This makes sure the skybox is rendered at the farthest depth.

}