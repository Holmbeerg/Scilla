// fragment shader/pixel shader, it computes color and other attributes of each pixel/fragment

#version 460 core

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse1; // sampler2D is an opaque type, cant instantiate. Tells the shader which texture unit to sample from
    sampler2D texture_specular1;
    float shininess;
};

// output color of the pixel/fragment
out vec4 FragColor;

// input data from the vertex shader
in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    // ambient. Ambient light is constant
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));

    // Implementation of Phong lighting

    // diffuse. Diffuse light depends on the angle between light source and surface normal
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords)).rgb;

    // specular. Specular light depends on the angle between view direction and reflection direction
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords)).rgb;

    // combine
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
