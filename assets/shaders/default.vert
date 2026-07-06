#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform float uOffset;
uniform mat4 transform;   // <-- Transformation matrix from C++

out vec3 ourColor;
out vec2 TexCoord;

void main() {
    // Apply transform to the position
    gl_Position = transform * vec4(aPos.x + uOffset, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}