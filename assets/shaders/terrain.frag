#version 460 core
out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

// Textures
uniform sampler2D grassTexture;
uniform sampler2D grassNormal;
uniform sampler2D rockTexture;
uniform sampler2D rockNormal;
uniform sampler2D snowTexture;
uniform sampler2D snowNormal;

// Camera
layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform vec3 u_SunDirection = vec3(0.3, 0.8, 0.4);

vec3 getSunColor(float sunHeight) {
    vec3 dayColor = vec3(1.0, 0.98, 0.95);
    vec3 sunsetColor = vec3(1.0, 0.6, 0.3);
    vec3 nightColor = vec3(0.3, 0.4, 0.6);

    float sunsetFactor = 1.0 - smoothstep(0.0, 0.3, abs(sunHeight));
    float dayFactor = smoothstep(-0.1, 0.3, sunHeight);

    vec3 color = mix(nightColor, sunsetColor, sunsetFactor);
    color = mix(color, dayColor, dayFactor);

    return color;
}

vec3 getSkyAmbient(float sunHeight) {
    // Match your sky shader's colors
    vec3 dayAmbient = vec3(0.5, 0.7, 1.0);
    vec3 sunsetAmbient = vec3(0.6, 0.4, 0.5);
    vec3 nightAmbient = vec3(0.1, 0.15, 0.3);

    float sunsetFactor = 1.0 - smoothstep(0.0, 0.3, abs(sunHeight));
    float dayFactor = smoothstep(-0.1, 0.3, sunHeight);

    vec3 ambient = mix(nightAmbient, sunsetAmbient, sunsetFactor);
    ambient = mix(ambient, dayAmbient, dayFactor);

    return ambient;
}

struct SurfaceMaterial {
    vec3 albedo;
    vec3 normal;
};

SurfaceMaterial sampleMaterial(sampler2D albedoTex, sampler2D normalTex, vec2 uv, mat3 TBN) {
    SurfaceMaterial mat;

    // Multi-scale sampling to break up tiling
    vec3 detail = texture(albedoTex, uv * 10.0).rgb;
    vec3 mid = texture(albedoTex, uv * 4.0).rgb;
    vec3 macro = texture(albedoTex, uv * 0.5).rgb;
    mat.albedo = detail * 0.5 + mid * 0.3 + macro * 0.2;

    // Sample normal map
    vec3 normalDetail = texture(normalTex, uv * 20.0).rgb * 2.0 - 1.0;
    vec3 normalMid = texture(normalTex, uv * 5.0).rgb * 2.0 - 1.0;
    vec3 tangentNormal = normalize(normalDetail * 0.7 + normalMid * 0.3);

    // Transform from tangent space to world space
    mat.normal = normalize(TBN * tangentNormal);

    return mat;
}

struct BlendedMaterial {
    vec3 albedo;
    vec3 normal;
};

BlendedMaterial getTerrainMaterial(float height, float slope, vec2 uv, mat3 TBN) {
    // Sample all three materials
    SurfaceMaterial grass = sampleMaterial(grassTexture, grassNormal, uv, TBN);
    SurfaceMaterial rock = sampleMaterial(rockTexture, rockNormal, uv, TBN);
    SurfaceMaterial snow = sampleMaterial(snowTexture, snowNormal, uv, TBN);

    // Calculate blend weights
    float grassWeight = 0.0;
    float rockWeight = 0.0;
    float snowWeight = 0.0;

    // Slope-based blending
    float slopeFactor = smoothstep(0.15, 0.5, slope);

    // Height-based blending with smooth transitions
    if (height < 8.0) {
        grassWeight = 1.0 - slopeFactor;
        rockWeight = slopeFactor;
    }
    else if (height < 18.0) {
        float heightBlend = smoothstep(8.0, 18.0, height);
        grassWeight = (1.0 - heightBlend) * (1.0 - slopeFactor * 0.8);
        rockWeight = heightBlend * 0.7 + slopeFactor * 0.8;
        snowWeight = heightBlend * 0.3 * (1.0 - slopeFactor);
    }
    else if (height < 32.0) {
        float snowBlend = smoothstep(18.0, 32.0, height);
        rockWeight = (1.0 - snowBlend) * (1.0 - slopeFactor * 0.3);
        snowWeight = snowBlend * (1.0 - slopeFactor * 0.6);
        rockWeight += slopeFactor * 0.6;
    }
    else {
        snowWeight = 1.0 - slopeFactor * 0.4;
        rockWeight = slopeFactor * 0.4;
    }

    // Normalize weights
    float totalWeight = grassWeight + rockWeight + snowWeight;
    grassWeight /= totalWeight;
    rockWeight /= totalWeight;
    snowWeight /= totalWeight;

    // Blend materials
    BlendedMaterial result;
    result.albedo = grass.albedo * grassWeight +
    rock.albedo * rockWeight +
    snow.albedo * snowWeight;
    result.normal = normalize(grass.normal * grassWeight +
    rock.normal * rockWeight +
    snow.normal * snowWeight);

    return result;
}

vec3 calculateLighting(vec3 albedo, vec3 normal, vec3 fragPos, float sunHeight) {
    vec3 N = normalize(normal);
    vec3 L = normalize(u_SunDirection);
    vec3 V = normalize(viewPos - fragPos);
    vec3 H = normalize(L + V);

    vec3 sunColor = getSunColor(sunHeight);
    vec3 skyColor = getSkyAmbient(sunHeight);

    float NdotL = dot(N, L);
    float diffuseWrap = max((NdotL + 0.3) / 1.3, 0.0);

    float sunIntensity = smoothstep(-0.2, 0.1, sunHeight);
    vec3 diffuse = diffuseWrap * sunColor * sunIntensity;

    float spec = pow(max(dot(N, H), 0.0), 64.0);
    vec3 specular = spec * sunColor * 0.15 * sunIntensity;

    float skyLight = max(dot(N, vec3(0, 1, 0)), 0.0) * 0.5 + 0.5;
    vec3 ambient = mix(skyColor * 0.05, skyColor * 0.2, skyLight);

    // Ambient occlusion based on slope
    float ao = 1.0 - (1.0 - N.y) * 0.3;

    vec3 lighting = (ambient * ao + diffuse + specular) * albedo;

    return lighting;
}

void main() {
    // Build TBN matrix for normal mapping
    vec3 T = normalize(Tangent);
    vec3 B = normalize(Bitangent);
    vec3 N = normalize(Normal);
    mat3 TBN = mat3(T, B, N);

    // Calculate slope
    float slope = 1.0 - Normal.y;

    // Get material properties
    BlendedMaterial material = getTerrainMaterial(Height, slope, TexCoords, TBN);

    // Calculate sun height
    float sunHeight = u_SunDirection.y;

    // Calculate lighting with time-of-day awareness
    vec3 litColor = calculateLighting(material.albedo, material.normal, FragPos, sunHeight);
    litColor *= 0.5;

    vec3 finalColor = litColor;

    // Tone mapping
    finalColor = finalColor / (finalColor + vec3(1.0));

    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0/2.2));

    FragColor = vec4(finalColor, 1.0);
}