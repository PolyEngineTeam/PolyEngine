#version 330

layout(location = 0) in vec4 aPos;

uniform mat4 uClipFromModel;

void main() {
    gl_Position = uClipFromModel * aPos;
}