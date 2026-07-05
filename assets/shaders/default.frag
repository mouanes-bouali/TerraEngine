#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

uniform vec4 uOverrideColor;
uniform sampler2D ourTexture;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
    // Multiply texture with vertex color and the time-override
    FragColor = texColor * vec4(ourColor, 1.0) * uOverrideColor;
}