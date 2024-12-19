// left triangle (small one)
#version 460 core
out vec4 FragColor;
uniform vec4 ourColor;  //color passed as uniform from code
in vec3 vertPos;

void main() {
    FragColor = vec4(vertPos, 1.0);  // use the uniform color instead of per-vertex color
}
