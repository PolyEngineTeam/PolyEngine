#version 430 core

uniform float uNear;
uniform float uFar;
uniform sampler2D uTexture;

in vec2 vUV;
out vec4 fragColor;

// Need to liuNearize the depth because we are using the projection
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
}

void main() {
	vec3 tex = texture(uTexture, vUV).rgb;
    float depth = LinearizeDepth(tex.r)/uFar;
    // tex = pow(tex, vec3(0.45));
    fragColor = vec4(vec3(depth), 1.0);
}