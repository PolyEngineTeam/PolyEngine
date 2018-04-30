#version 430

struct Light
{
    vec4 Position;
    float Radius;
};

struct VisibleIndex {
	int index;
};

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
    vec4 tilePosSS;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

layout(std430, binding = 2) writeonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;


const uint NUM_LIGHTS = 1024;
// const vec2 ScreenSize = vec2(800.0, 600.0);
const vec2 ScreenSize = vec2(1920.0, 1080.0);

// Uniforms
uniform sampler2D depthMap;
uniform float time;
uniform float near;
uniform float far;
uniform mat4 ViewFromWorld;
uniform mat4 ClipFromWorld;
uniform mat4 ClipFromView;
uniform int screenSizeX;
uniform int screenSizeY;
uniform int workGroupsX;
uniform int workGroupsY;
uniform int lightCount;

shared uint minDepthInt = 0xFFFFFFFF;
shared uint maxDepthInt = 0;
shared uint visibleLightCount = 0;
shared vec4 frustumPlanes[6];
shared int visibleLightIndices[NUM_LIGHTS];

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main()
{
    uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    
    mat4 view = ViewFromWorld;
	mat4 projection = ClipFromView;
    mat4 viewProjection = ClipFromWorld;

    ivec2 location = ivec2(gl_GlobalInvocationID.xy);
    ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
    ivec2 tileID = ivec2(gl_WorkGroupID.xy);
    ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
    uint index = tileID.y * tileNumber.x + tileID.x;

	// Initialize shared global values for depth and light count
    if (gl_LocalInvocationIndex == 0)
    {
        minDepthInt = 0xFFFFFFFF;
        maxDepthInt = 0;
        visibleLightCount = 0;

        for (uint i = 0; i < NUM_LIGHTS; i++)
        {
            visibleLightIndices[i] = int(-1);
        }
    }

    barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
    float maxDepth, minDepth;
    vec2 text = vec2(location) / ScreenSize;
    float depth = texture(depthMap, text).r;
	// Linearize the depth value from depth buffer (must do this because we created it using projection)
    depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

	// Convert depth to uint so we can do atomic min and max comparisons between the threads
    uint depthInt = floatBitsToUint(depth);
    atomicMin(minDepthInt, depthInt);
    atomicMax(maxDepthInt, depthInt);

    barrier();

	// Step 2: One thread should calculate the frustum planes to be used for this tile
    if (gl_LocalInvocationIndex == 0)
    {
		// Convert the min and max across the entire tile back to float
        minDepth = uintBitsToFloat(minDepthInt);
        maxDepth = uintBitsToFloat(maxDepthInt);

		// Steps based on tile sale
        vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);
        vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

		// Set up starting values for planes using steps and min and max z values
        frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
        frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
        frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
        frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
        frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
        frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far
        
		// Transform the first four planes
        for (uint i = 0; i < 4; i++)
        {
            frustumPlanes[i] *= viewProjection;
            frustumPlanes[i] /= length(frustumPlanes[i].xyz);
        }

		// Transform the depth planes
        frustumPlanes[4] *= view;
        frustumPlanes[4] /= length(frustumPlanes[4].xyz);
        frustumPlanes[5] *= view;
        frustumPlanes[5] /= length(frustumPlanes[5].xyz);
    }

    barrier();

    if (gl_LocalInvocationIndex == 0)
    {
        for (uint i = 0; i < NUM_LIGHTS; i++)
        {
            vec4 position = lightBuffer.data[i].Position;
            float radius = lightBuffer.data[i].Radius;
	
			// We check if the light exists in our frustum
            float distance = 0.0;
            for (uint j = 0; j < 6; j++)
            {
                distance = dot(position, frustumPlanes[j]) + radius;
	
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
                // uint offset = atomicAdd(visibleLightCount, floatBitsToUint(1.0));
                uint offset = visibleLightCount;
                visibleLightIndices[offset] = int(i);
                visibleLightCount += 1;
            }
        }
    }

	barrier();

	if (gl_LocalInvocationIndex == 0)
	{
        uint offset = index * NUM_LIGHTS; // Determine position in global buffer

		// for (uint i = 0; i < NUM_LIGHTS; i++)
        // {
        //     visibleLightIndicesBuffer.data[offset + i].index = int(i);
        // }

        // uint range = uint(visibleLightCount);
        // for (uint i = 0; i < range; i++)
        // {
        //     visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
        // }

        // visibleLightIndicesBuffer.data[offset + 0].index = visibleLightIndices[0];
        // visibleLightIndicesBuffer.data[offset + 1].index = visibleLightIndices[1];
        // visibleLightIndicesBuffer.data[offset + 2].index = visibleLightIndices[2];
        // visibleLightIndicesBuffer.data[offset + 3].index = visibleLightIndices[3];
        // visibleLightIndicesBuffer.data[offset + 4].index = visibleLightIndices[4];
        // visibleLightIndicesBuffer.data[offset + 5].index = visibleLightIndices[5];
        // visibleLightIndicesBuffer.data[offset + 6].index = visibleLightIndices[6];
        // visibleLightIndicesBuffer.data[offset + 7].index = visibleLightIndices[7];
        // visibleLightIndicesBuffer.data[offset + 8].index = visibleLightIndices[8];
        // visibleLightIndicesBuffer.data[offset + 9].index = visibleLightIndices[9];

        // visibleLightIndicesBuffer.data[offset].index = int(visibleLightCount);

        for (uint i = 0; i < NUM_LIGHTS; i++)
        {
            visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
        }
		 
        // if (visibleLightCount != NUM_LIGHTS)
        // {
		// 	// Unless we have totally filled the entire array, mark it's end with -1
		// 	// Final shader step will use this to determine where to stop (without having to pass the light count)
        //     visibleLightIndicesBuffer.data[offset + visibleLightCount].index = int(-1);
        // }

		outputBuffer.data[IndexWorkGroup].indexLocal = 0;
		outputBuffer.data[IndexWorkGroup].indexWorkGroup = IndexWorkGroup;
		outputBuffer.data[IndexWorkGroup].indexGlobal = 0; // IndexGlobal;
		outputBuffer.data[IndexWorkGroup].input = maxDepthInt;
		outputBuffer.data[IndexWorkGroup].result = visibleLightCount;
	}
}