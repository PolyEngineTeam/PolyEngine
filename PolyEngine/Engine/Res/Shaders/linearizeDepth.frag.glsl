#version 330 core

in vec2 vUV;

uniform sampler2D uDepth;
uniform float uNear;
uniform float uFar;

out vec4 oColor;

float LinearizeDepth(float depth) {
	// float z = depth * 2.0 - 1.0;	
	// return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
	return -uFar * uNear / (depth * (uFar - uNear) - uFar);
}

void main()
{
    float depth = texture(uDepth, vUV).r;
    // oColor = vec4(LinearizeDepth(depth), 0.0, 0.0, 1.0);	
	oColor = vec4(depth, LinearizeDepth(depth)/uFar, 0.0, 1.0 );
}