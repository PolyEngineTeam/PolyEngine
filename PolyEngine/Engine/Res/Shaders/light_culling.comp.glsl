#version 430

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
};

layout(std430, binding = 0) writeonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

// Uniforms
uniform sampler2D depthMap;
uniform int screenSizeX;
uniform int screenSizeY;

uniform float near;
uniform float far;

shared uint minDepthInt;
shared uint maxDepthInt;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main() {

    uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    uint IndexGlobal = gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x) + gl_GlobalInvocationID.x;

	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
	}

	barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
	float maxDepth, minDepth;
    ivec2 location = ivec2(gl_GlobalInvocationID.xy);
    ivec2 screenSize = ivec2(screenSizeX, screenSizeY);
	vec2 uv = vec2(location) / screenSize;
	float depth = texture(depthMap, uv).r;
    depth = LinearizeDepth(depth) / far;
    
	uint depthInt = floatBitsToUint(depth);
    outputBuffer.data[IndexWorkGroup].input = depthInt;

    barrier();
	
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
	
	barrier();

    outputBuffer.data[IndexWorkGroup].indexLocal = gl_LocalInvocationIndex;
    outputBuffer.data[IndexWorkGroup].indexWorkGroup = IndexWorkGroup;
    outputBuffer.data[IndexWorkGroup].indexGlobal = 0; // IndexGlobal;
    outputBuffer.data[IndexWorkGroup].result = minDepthInt;
}
