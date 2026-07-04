#version 330 core
in vec3 ourColor;
uniform vec4 uOverrideColor;

out vec4 FragColor;

void main() {
    // Combine vertex color with the overriding uniform
    FragColor = vec4(ourColor, 1.0) * uOverrideColor;
}