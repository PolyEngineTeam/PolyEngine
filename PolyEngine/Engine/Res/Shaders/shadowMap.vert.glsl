#version 330

layout(location = 0) in vec4 aPos;
// layout(location = 1) in vec2 aTexCoord;
// layout(location = 2) in vec3 aNormal;

uniform mat4 uClipFromModel;

void main() {
    gl_Position = uClipFromModel * aPos;
}