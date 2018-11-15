#version 330

layout(location = 0) in vec4 aPos;

uniform mat4 uClipFromModel;

out vec4 vPosition;

void main() {
	vPosition = uClipFromModel * aPos;
    gl_Position = vPosition;
}