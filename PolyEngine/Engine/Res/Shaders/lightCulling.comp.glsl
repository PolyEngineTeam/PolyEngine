#version 430
// based on https://github.com/bcrusco/Forward-Plus-Renderer

struct Light
{
	vec4 Position;
	vec4 Color;
	vec4 RangeIntensity;
};

struct VisibleIndex
{
	int Index;
};

layout(std430, binding = 0) readonly buffer LightBuffer
{
	Light bLights[];
};

layout(std430, binding = 1) writeonly buffer VisibleLightIndicesBuffer
{
	VisibleIndex bVisibleIndicies[];
};

const uint MAX_NUM_LIGHTS = 1024;

uniform sampler2D uDepthMap;
uniform mat4 uViewFromWorld;
uniform mat4 uClipFromWorld;
uniform mat4 uClipFromView;
uniform int uScreenSizeX;
uniform int uScreenSizeY;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;
uniform int uLightCount;

shared uint sMinDepthInt;
shared uint sMaxDepthInt;
shared uint sVisibleLightCount;
shared vec4 sFrustumPlanes[6];
shared int sVisibleLightIndices[MAX_NUM_LIGHTS];

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main()
{
	vec2 ScreenSize = vec2(uScreenSizeX, uScreenSizeY);
	ivec2 FragCoord = ivec2(gl_GlobalInvocationID.xy);
	uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;

	if (gl_LocalInvocationIndex == 0)
	{
		sMinDepthInt = 0xFFFFFFFF;
		sMaxDepthInt = 0;
		sVisibleLightCount = 0;
	}

	barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
	float maxDepth, minDepth;
	vec2 text = vec2(FragCoord) / ScreenSize;
	float depth = texture(uDepthMap, text).r;
	// Linearize the depth value from depth buffer (must do this because we created it using projection)
	depth = (0.5 * uClipFromView[3][2]) / (depth + 0.5 * uClipFromView[2][2] - 0.5);

	// Convert depth to uint so we can do atomic min and max comparisons between the threads
	uint depthInt = floatBitsToUint(depth);
	atomicMin(sMinDepthInt, depthInt);
	atomicMax(sMaxDepthInt, depthInt);

	barrier();

	// Step 2: One thread should calculate the frustum planes to be used for this tile
	if (gl_LocalInvocationIndex == 0)
	{
		ivec2 TileID = ivec2(gl_WorkGroupID.xy);
		ivec2 TileNumber = ivec2(gl_NumWorkGroups.xy);

		// Convert the min and max across the entire tile back to float
		minDepth = uintBitsToFloat(sMinDepthInt);
		maxDepth = uintBitsToFloat(sMaxDepthInt);

		// Steps based on tile side
		vec2 negativeStep = (2.0 * vec2(TileID)) / vec2(TileNumber);
		vec2 positiveStep = (2.0 * vec2(TileID + ivec2(1, 1))) / vec2(TileNumber);

		// Set up starting values for planes using steps and min and max z values
		sFrustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
		sFrustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
		sFrustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
		sFrustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
		sFrustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
		sFrustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far
		
		// Transform the first four planes
		for (uint i = 0; i < 4; i++)
		{
			sFrustumPlanes[i] *= uClipFromWorld;
			sFrustumPlanes[i] /= length(sFrustumPlanes[i].xyz);
		}

		// Transform the depth planes
		sFrustumPlanes[4] *= uViewFromWorld;
		sFrustumPlanes[4] /= length(sFrustumPlanes[4].xyz);
		sFrustumPlanes[5] *= uViewFromWorld;
		sFrustumPlanes[5] /= length(sFrustumPlanes[5].xyz);
	}

	barrier();

	uint threadCount = TILE_SIZE * TILE_SIZE;
	uint passCount = (uLightCount + threadCount - 1) / threadCount;
	for (uint i = 0; i < passCount; i++)
	{
		// Get the lightIndex to test for this thread / pass. If the index is >= light count, then this thread can stop testing lights
		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
		if (lightIndex >= uLightCount)
		{
			break;
		}

		vec4 position = bLights[lightIndex].Position;
		float radius = bLights[lightIndex].RangeIntensity.x;
	
		// We check if the light exists in our frustum
		float distance = 0.0;
		for (uint j = 0; j < 6; ++j)
		{
			distance = dot(position, sFrustumPlanes[j]) + radius;
	
			// If one of the tests fails, then there is no intersection
			if (distance <= 0.0)
			{
				break;
			}
		}
	
		// If greater than zero, then it is a visible light
		if (distance > 0.0)
		{
			// Add index to the shared array of visible indices
			uint offset = atomicAdd(sVisibleLightCount, 1);
			sVisibleLightIndices[offset] = int(lightIndex);
		}
	}

	barrier();

	if (gl_LocalInvocationIndex == 0)
	{
		uint offset = IndexWorkGroup * MAX_NUM_LIGHTS; // Determine position in global buffer

		for (uint i = 0; i < sVisibleLightCount; ++i)
		{
			bVisibleIndicies[offset + i].Index = sVisibleLightIndices[i];
		}
		 
		if (sVisibleLightCount != MAX_NUM_LIGHTS)
		{
			// Unless we have totally filled the entire array, mark it's end with -1
			// Final shader step will use this to determine where to stop (without having to pass the light count)
			bVisibleIndicies[offset + sVisibleLightCount].Index = int(-1);
		}
	}
}