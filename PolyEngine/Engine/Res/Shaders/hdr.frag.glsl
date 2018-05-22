#version 430 core

in vec2 vUV;

// Uniforms
// Texture for the hdr buffer
uniform sampler2D hdrBuffer;

// Controls exposure level of image
uniform float uExposure;

out vec4 fragColor;

// Uses Reinhard tonemapping https://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
// with an added controllable exposure component

void main()
{
    vec3 color = texture(hdrBuffer, vUV).rgb;
    // vec3 color = texelFetch(hdrBuffer, vUV, 0).rgb;
    vec3 result = vec3(1.0) - exp(-color * uExposure);

	// Minor gamma correction. Need to expand on it
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}