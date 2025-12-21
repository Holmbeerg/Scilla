#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float Height;

uniform sampler2D grassTexture;
uniform sampler2D grassNormal;
uniform sampler2D grassAO;

uniform sampler2D rockTexture;
uniform sampler2D rockNormal;
uniform sampler2D rockAO;

uniform sampler2D snowTexture;
uniform sampler2D snowNormal;
uniform sampler2D snowAO;

uniform vec3 u_SunDirection; // normalized
uniform float u_RockHeight;
uniform float u_SnowHeight;

layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

struct Surface {
    vec3 albedo;
    vec3 normal;
    float ao;
};

vec3 triplanarBlend(vec3 n) {
    vec3 w = abs(n);
    w = max(w, 0.00001);
    return w / (w.x + w.y + w.z);
}

vec3 sampleTriplanarColor(sampler2D tex, vec3 p, vec3 n, float scale) {
    vec3 w = triplanarBlend(n);

    vec3 x = texture(tex, p.yz * scale).rgb;
    vec3 y = texture(tex, p.xz * scale).rgb;
    vec3 z = texture(tex, p.xy * scale).rgb;

    return x * w.x + y * w.y + z * w.z;
}

vec3 sampleTriplanarNormal(sampler2D tex, vec3 p, vec3 n, float scale) {
    vec3 w = triplanarBlend(n);

    vec3 nx = texture(tex, p.yz * scale).xyz * 2.0 - 1.0;
    vec3 ny = texture(tex, p.xz * scale).xyz * 2.0 - 1.0;
    vec3 nz = texture(tex, p.xy * scale).xyz * 2.0 - 1.0;

    nx = vec3(0.0, nx.y, nx.x);
    ny = vec3(ny.x, 0.0, ny.y);
    nz = vec3(nz.x, nz.y, 0.0);

    return normalize(nx * w.x + ny * w.y + nz * w.z);
}

Surface sampleSurface(
sampler2D albedo,
sampler2D normal,
sampler2D ao,
vec3 p,
vec3 n
) {
    const float SCALE = 0.05;

    Surface s;
    s.albedo = sampleTriplanarColor(albedo, p, n, SCALE);
    s.normal = sampleTriplanarNormal(normal, p, n, SCALE);
    s.ao     = clamp(sampleTriplanarColor(ao, p, n, SCALE).r, 0.4, 1.0);

    return s;
}

Surface blendTerrain(vec3 p, vec3 n) {

    Surface grass = sampleSurface(grassTexture, grassNormal, grassAO, p, n);
    Surface rock  = sampleSurface(rockTexture,  rockNormal,  rockAO,  p, n);
    Surface snow  = sampleSurface(snowTexture,  snowNormal,  snowAO,  p, n);

    float slope = smoothstep(0.2, 0.8, 1.0 - abs(n.y));

    float noise = fract(sin(dot(p.xz, vec2(12.9898, 78.233))) * 43758.5453);
    float h = Height + noise * 1.5;

    float grassW = 1.0 - smoothstep(u_RockHeight - 2.0, u_RockHeight + 2.0, h);
    float snowW  = smoothstep(u_SnowHeight - 2.0, u_SnowHeight + 2.0, h);
    float rockW  = 1.0 - grassW - snowW;

    grassW *= (1.0 - slope);
    snowW  *= (1.0 - slope);
    rockW  += slope;

    float sum = grassW + rockW + snowW;
    grassW /= sum;
    rockW  /= sum;
    snowW  /= sum;

    Surface s;
    s.albedo = grass.albedo * grassW + rock.albedo * rockW + snow.albedo * snowW;
    s.normal = normalize(grass.normal * grassW + rock.normal * rockW + snow.normal * snowW);
    s.ao     = grass.ao * grassW     + rock.ao * rockW     + snow.ao * snowW;

    return s;
}

vec3 lighting(Surface s, vec3 baseNormal) {

    vec3 N = normalize(s.normal);
    vec3 L = normalize(-u_SunDirection);

    float NdotL = max(dot(N, L), 0.0);

    // Hemisphere ambient
    vec3 sky = vec3(0.45, 0.55, 0.75);
    vec3 ground = vec3(0.15, 0.12, 0.1);
    float hemi = clamp(baseNormal.y, 0.0, 1.0);

    vec3 ambient = s.albedo * mix(ground, sky, hemi) * 0.6 * s.ao;

    // Lambert diffuse
    vec3 sunColor = vec3(1.0, 0.97, 0.9);
    vec3 diffuse = s.albedo * sunColor * NdotL;

    return ambient + diffuse;
}

void main() {

    vec3 baseNormal = normalize(Normal);
    Surface surface = blendTerrain(FragPos, baseNormal);

    vec3 color = lighting(surface, baseNormal);

    FragColor = vec4(color, 1.0);
}
