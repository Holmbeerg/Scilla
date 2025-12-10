// Procedural Sky Shader with Day, Sunset, and Night Transitions
// This works by that our vertex shader is a cube mapped around the camera
// and we shade each fragment based on its direction vector.

// Inputs:
// - u_SunDirection: A normalized vec3 indicating the sun's direction in world space.

#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform vec3 u_SunDirection;

// Colors
vec3 dayTop = vec3(0.2, 0.6, 1.0);
vec3 dayHorizon = vec3(0.7, 0.8, 0.9);
vec3 sunsetTop = vec3(0.3, 0.2, 0.5);      // Purple
vec3 sunsetHorizon = vec3(1.0, 0.4, 0.2);  // Orange/Red
vec3 nightTop = vec3(0.02, 0.02, 0.1);
vec3 nightHorizon = vec3(0.1, 0.1, 0.2);

const float NOISE_GRANULARITY = 0.5/255.0; // Adjust this value to change the dither intensity

// A standard pseudo-random function for shaders https://thebookofshaders.com/10/
// We use this for dithering to reduce colour banding (had this before) https://en.wikipedia.org/wiki/Colour_banding
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec3 viewDir = normalize(localPos);

    // Vertical gradient (0 = horizon, 1 = zenith)
    float verticalPos = viewDir.y;
    float horizonFade = 1.0 - exp(-abs(verticalPos) * 2.0);

    float sunHeight = u_SunDirection.y;

    // Calculate transition factors
    // sunsetFactor: peaks when sun is at horizon (sunHeight ≈ 0)
    float sunsetFactor = 1.0 - smoothstep(0.0, 0.3, abs(sunHeight));
    // dayFactor: 0 at night, 1 during day
    float dayFactor = smoothstep(-0.1, 0.3, sunHeight);

    // Mix colors based on vertical position
    vec3 dayColor = mix(dayHorizon, dayTop, horizonFade);
    vec3 sunsetColor = mix(sunsetHorizon, sunsetTop, horizonFade);
    vec3 nightColor = mix(nightHorizon, nightTop, horizonFade);

    // Three-way blend: night -> sunset -> day
    vec3 finalSky = mix(nightColor, sunsetColor, sunsetFactor);
    finalSky = mix(finalSky, dayColor, dayFactor);

    // Draw the sun (only when above horizon)
    if (sunHeight > -0.1) {
        float sunDot = dot(viewDir, u_SunDirection);
        float sunMask = smoothstep(0.995, 0.998, sunDot);
        float sunGlow = smoothstep(0.98, 0.995, sunDot);

        vec3 sunColor = vec3(1.0, 1.0, 0.8);
        vec3 glowColor = vec3(1.0, 0.6, 0.1);

        finalSky = mix(finalSky, sunColor, sunMask);
        finalSky += glowColor * sunGlow * 0.5;
    }

    // Dither to reduce banding. Dithering is the process of introducing noise into an image.
    // Generate noise based on screen coordinates (gl_FragCoord)
    float noise = random(gl_FragCoord.xy);

    // Mix the noise into the final color
    // We map noise from [0,1] to [-1/255, 1/255] for subtle effect
    // Break up 8-bit color banding
    finalSky += mix(-NOISE_GRANULARITY, NOISE_GRANULARITY, noise);

    FragColor = vec4(finalSky, 1.0);
}