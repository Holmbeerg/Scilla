#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
    sampler2D arm;
};

uniform Material material;
uniform vec3 u_SunDirection;

layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

void main() {
    // Sample Diffuse Texture
    vec4 texColor = texture(material.diffuse, TexCoords);
    if (texColor.a < 0.1) discard;

    // Sample ARM/ORM Texture
    vec3 ormSample = texture(material.arm, TexCoords).rgb;
    float ao = max(ormSample.r, 0.3);
    float roughness = ormSample.g;
    float metallic = ormSample.b;

    // Normal Mapping
    vec3 normalMapSample = texture(material.normal, TexCoords).rgb;
    normalMapSample = normalMapSample * 2.0 - 1.0;  // [0,1] -> [-1,1]
    vec3 transformedNormal = TBN * normalMapSample; // Transform to world space
    vec3 normal = normalize(transformedNormal);

    // Lighting Calculations
    vec3 lightDir = normalize(u_SunDirection);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Ambient Occlusion
    vec3 ambient = 0.5 * texColor.rgb * ao;

    // Diffuse
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = NdotL * texColor.rgb;

    // Specular (Blinn-Phong)
    float shininess = (1.0 - roughness) * 128.0;
    float spec = pow(
    max(dot(normal, halfwayDir), 0.0001),
    max(shininess, 0.0001)
    );
    vec3 specular = vec3(0.2) * spec * (1.0 - roughness);

    // Combine
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}