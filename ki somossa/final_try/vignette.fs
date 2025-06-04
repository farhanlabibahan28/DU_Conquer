#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform vec2 characterScreenPos;

void main() {
    vec2 pixelCoord = fragTexCoord * resolution;

    float dist = distance(pixelCoord, characterScreenPos);

    // Radius range for smooth fade
    float vignette = smoothstep(200.0, 600.0, dist);

    vec4 texColor = texture(texture0, fragTexCoord);
    finalColor = mix(texColor, vec4(0.0, 0.0, 0.0, 1.0), vignette);
}
