#version 430 core

in vec2 TextureCoordinates;

// Uniforms
// Texture for the hdr buffer
uniform sampler2D hdrBuffer;

uniform float near;
uniform float far;

// Controls exposure level of image
uniform float exposure;

out vec4 fragColor;

// Uses Reinhard tonemapping https://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
// with an added controllable exposure component

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	// vec3 color = texture(hdrBuffer, TextureCoordinates).rgb;
	// vec3 result = vec3(1.0) - exp(-color * exposure);

	// Minor gamma correction. Need to expand on it
	// const float gamma = 2.2;
	// result = pow(result, vec3(1.0 / gamma));
	// fragColor = vec4(result, 1.0);
    float depthTex = texture(hdrBuffer, TextureCoordinates).r;
    float depth = LinearizeDepth(depthTex) / far;
    fragColor = vec4(vec3(depth), 1.0);
}