#version 430 core

in	vec2 TextureCoordinates;

struct PointLight {
    vec4 color;
    vec4 position;
    vec4 paddingAndRadius;
};

struct VisibleIndex {
    int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

// Uniforms
// Texture for the hdr buffer
uniform sampler2D hdrBuffer;

uniform float near;
uniform float far;

uniform float numberOfTilesX;
uniform float totalLightCount;

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
    // fragColor = vec4(tex, 1.0);


	// Determine which tile this pixel belongs to
    ivec2 location = ivec2(gl_FragCoord.xy);
    ivec2 tileID = location / ivec2(16, 16);
    uint index = tileID.y * uint(numberOfTilesX) + tileID.x;
	
    uint offset = index * 2;
    uint i;
    // for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);
    i = visibleLightIndicesBuffer.data[offset].index;
    float depthComp = visibleLightIndicesBuffer.data[offset+1].index;
	 
    // float ratio = fract(float(i)/far); // / totalLightCount;
    // fragColor = vec4(vec3(ratio), 1.0);
    // fragColor = vec4(float(index) / (800.0 / 16.0 * 600.0 / 16.0), 0.0, 0.0, 1.0);
    // fragColor = vec4(vec3(float(i) / ((800.0 / 16.0) * (600.0 / 16.0))), 1.0);
    fragColor = vec4(vec3(LinearizeDepth(depthComp)), 1.0);
}