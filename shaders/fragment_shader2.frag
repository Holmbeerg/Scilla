// right triangle
#version 460 core
out vec4 FragColor;
uniform vec4 ourColor;  //color passed as uniform from code
in vec2 texCoord;

uniform sampler2D ourTexture;

void main() {
    FragColor = texture(ourTexture, texCoord);
}
