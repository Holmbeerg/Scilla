// right triangle
#version 460 core
out vec4 FragColor;
//uniform vec4 ourColor;  //color passed as uniform from code
in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
    FragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(1.0 - texCoord.x, texCoord.y)), 0.2);
}
