#version 460 core
out vec4 FragColor;
uniform vec4 ourColor;  //color passed as uniform from code

void main() {
    FragColor = ourColor;  // use the uniform color instead of per-vertex color
}
