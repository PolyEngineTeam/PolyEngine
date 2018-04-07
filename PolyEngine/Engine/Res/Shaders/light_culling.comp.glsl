#version 430

struct Light
{
    vec4 Position;
    float Radius;
};

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

// Uniforms
uniform sampler2D depthMap;
uniform int screenSizeX;
uniform int screenSizeY;
uniform float near;
uniform float far;
uniform mat4 ViewFromWorld;
uniform mat4 ScreenFromView;
uniform int lightCount;

shared uint minDepthInt;
shared uint maxDepthInt;
shared vec4 frustumPlanes[6];
shared mat4 ScreenFromWorld;
shared uint visibleLightCount;

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
        ScreenFromWorld = ScreenFromView * ViewFromWorld;
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

    barrier();
	
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
	
	barrier();

	// // Step 2: One thread should calculate the frustum planes to be used for this tile
    // if (gl_LocalInvocationIndex == 0)
    // {
	// 	// Convert the min and max across the entire tile back to float
    //     minDepth = uintBitsToFloat(minDepthInt);
    //     maxDepth = uintBitsToFloat(maxDepthInt);
	// 
	// 	// Steps based on tile sale
    //     vec2 negativeStep = (2.0 * vec2(gl_WorkGroupID.xy)) / vec2(gl_NumWorkGroups.xy);
    //     vec2 positiveStep = (2.0 * vec2(gl_WorkGroupID.xy + ivec2(1, 1))) / vec2(gl_NumWorkGroups.xy);
	// 
	// 	// Set up starting values for planes using steps and min and max z values
    //     frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
    //     frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
    //     frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
    //     frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
    //     frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
    //     frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far
	// 
	// 	// Transform the first four planes
    //     for (uint i = 0; i < 4; i++)
    //     {
    //         frustumPlanes[i] *= ScreenFromWorld;
    //         frustumPlanes[i] /= length(frustumPlanes[i].xyz);
    //     }
	// 
	// 	// Transform the depth planes
    //     frustumPlanes[4] *= ViewFromWorld;
    //     frustumPlanes[4] /= length(frustumPlanes[4].xyz);
    //     frustumPlanes[5] *= ViewFromWorld;
    //     frustumPlanes[5] /= length(frustumPlanes[5].xyz);
    // }

	
    // barrier();
	// 
	// // Step 3: Cull lights.
	// // Parallelize the threads against the lights now.
	// // Can handle 256 simultaniously. Anymore lights than that and additional passes are performed
    // uint threadCount = TILE_SIZE * TILE_SIZE;
    // uint passCount = (lightCount + threadCount - 1) / threadCount;
    // for (uint i = 0; i < passCount; i++)
    // {
	// 	// Get the lightIndex to test for this thread / pass. If the index is >= light count, then this thread can stop testing lights
    //     uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
    //     if (lightIndex >= lightCount)
    //     {
    //         break;
    //     }
	// 
    //     vec4 position = lightBuffer.data[lightIndex].Position;
    //     float radius = lightBuffer.data[lightIndex].Radius;
	// 
	// 	// We check if the light exists in our frustum
    //     float distance = 0.0;
    //     for (uint j = 0; j < 6; j++)
    //     {
    //         distance = dot(position, frustumPlanes[j]) + radius;
	// 
	// 		// If one of the tests fails, then there is no intersection
    //         if (distance <= 0.0)
    //         {
    //             break;
    //         }
    //     }
	// 
	// 	// If greater than zero, then it is a visible light
    //     if (distance > 0.0)
    //     {
	// 		// Add index to the shared array of visible indices
    //         uint offset = atomicAdd(visibleLightCount, 1);
    //         // visibleLightIndices[offset] = int(lightIndex);
    //     }
    // }
	// 
    // barrier();

    if (gl_LocalInvocationIndex == 0)
    {
        outputBuffer.data[IndexWorkGroup].indexLocal = 0;
        outputBuffer.data[IndexWorkGroup].indexWorkGroup = IndexWorkGroup;
        outputBuffer.data[IndexWorkGroup].indexGlobal = 0; // IndexGlobal;
        outputBuffer.data[IndexWorkGroup].input = minDepthInt;
        outputBuffer.data[IndexWorkGroup].result = visibleLightCount;
    }
}
