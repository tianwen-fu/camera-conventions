#version 330 core
in vec3 position;
in vec3 color;

out vec4 vertColor;

uniform mat4 extrinsics;

void main() {
    gl_Position = extrinsics * vec4(position, 1.0f);
    vertColor = vec4(color, 1.0f);
}