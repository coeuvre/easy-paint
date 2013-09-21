#version 330 core

uniform mat4 mmvp;

layout(location = 0) in vec2 vposition;
layout(location = 1) in vec2 vtexcoord;

out vec2 texcoord;

void main(void) {
    gl_Position = vec4(vposition, 0, 1);
    texcoord = vtexcoord;
}
