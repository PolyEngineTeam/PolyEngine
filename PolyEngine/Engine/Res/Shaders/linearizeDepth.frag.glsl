#version 330 core

in vec2 vUV;

uniform sampler2D uDepth;
uniform float uNear;
uniform float uFar;

out vec4 oColor;

float LinearizeDepth(float depth) {
	return -uFar * uNear / (depth * (uFar - uNear) - uFar);
}

void main()
{
    float depth = texture(uDepth, vUV).r;
	oColor = vec4(depth, LinearizeDepth(depth)/uFar, 0.0, 1.0 );
}