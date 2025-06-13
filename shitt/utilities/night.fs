#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 tint;
uniform vec2 resolution;
uniform vec2 lightPos;

out vec4 finalColor;

void main() {
    vec2 uv = fragTexCoord;
    vec4 texColor = texture(texture0, uv);

    // Slight night tint (so it's still visible but dusk-toned)
    vec3 nightTint = vec3(0.4, 0.4, 0.55);  // slightly brighter navy-purple
    texColor.rgb *= nightTint;

    // Glow falloff around ghost
    vec2 fragScreenPos = uv * resolution;
    vec2 lightScreenPos = lightPos * resolution;
    float distToLight = distance(fragScreenPos, lightScreenPos);

    // Glow is sharper and smaller (gradient falloff, smooth in center)
    float glow = pow(1.0 - smoothstep(60.0, 200.0, distToLight), 2.0);
    texColor.rgb += glow * vec3(0.3, 0.2, 0.5); // purple-ish glow

    // Softer vignette for subtle edge darkness
    vec2 centered = uv - 0.5;
    float vignette = smoothstep(0.85, 0.4, length(centered));
    texColor.rgb *= mix(0.85, 1.0, vignette);

    finalColor = vec4(texColor.rgb, 1.0);
}
