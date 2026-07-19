#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// Instanced attributes (one per instance, not per vertex)
layout (location = 3) in mat4 instanceModel;
layout (location = 7) in vec4 instanceColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;
out vec2 TexCoord;

void main() {
    vec4 worldPos = instanceModel * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    ourColor = instanceColor.rgb;
    TexCoord = aTexCoord;
}