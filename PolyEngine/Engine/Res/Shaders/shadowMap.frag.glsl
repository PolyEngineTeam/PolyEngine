#version 330

in vec4 vPosition;

out vec4 oColor;

void main() {
	float depth = vPosition.z / vPosition.w;
	depth = depth * 0.5 + 0.5;
	oColor = vec4(depth, 0.0, 0.0, 0.0);
}
