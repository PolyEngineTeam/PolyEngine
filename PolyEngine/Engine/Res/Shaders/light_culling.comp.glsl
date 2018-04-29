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


const uint NUM_LIGHTS = 25;

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

shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;

shared vec4 frustumPlanes[6];
shared int visibleLightIndices[NUM_LIGHTS];

vec2 ScreenSize = vec2(800.0, 600.0);

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float LinearizeDepth2(float depth)
{
    return (0.5 * ClipFromView[3][2]) / (depth + 0.5 * ClipFromView[2][2] - 0.5);
}

vec4 ScreenFromClip(vec4 posInClip)
{
    vec4 posInScreen = vec4(((posInClip.xyz / posInClip.w) + 1.0) * 0.5, 1.0);
    posInScreen.xy *= ScreenSize;
    return posInScreen;
}

// return 1 if v inside the box, return 0 otherwise
float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight)
{
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;
}

float intersectionAABBAABB(vec3 aMin, vec3 aMax, vec3 bMin, vec3 bMax)
{
	// TODO: branchless like insideBox
    if (aMax.x < bMin.x || aMin.x > bMax.x) return 0.0;
    if (aMax.y < bMin.y || aMin.y > bMax.y) return 0.0;
    if (aMax.z < bMin.z || aMin.z > bMax.z) return 0.0;
    return 1.0;
}

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
        // viewProjection = projection * view;
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
                uint offset = atomicAdd(visibleLightCount, floatBitsToUint(1.0));
				visibleLightIndices[offset] = int(i);
            }
        }
    }

	barrier();

	if (gl_LocalInvocationIndex == 0)
	{
        uint offset = index * NUM_LIGHTS; // Determine position in global buffer
		visibleLightIndicesBuffer.data[offset].index = visibleLightIndices[0];

        // for (uint i = 0; i < visibleLightCount; i++)
        // {
		//	visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
        // }
		 
        // if (visibleLightCount != NUM_LIGHTS)
        // {
		//  	// Unless we have totally filled the entire array, mark it's end with -1
		//  	// Final shader step will use this to determine where to stop (without having to pass the light count)
        //      visibleLightIndicesBuffer.data[offset + visibleLightCount].index = -1;
        // }

		outputBuffer.data[IndexWorkGroup].indexLocal = 0;
		outputBuffer.data[IndexWorkGroup].indexWorkGroup = IndexWorkGroup;
		outputBuffer.data[IndexWorkGroup].indexGlobal = 0; // IndexGlobal;
		outputBuffer.data[IndexWorkGroup].input = maxDepthInt;
		outputBuffer.data[IndexWorkGroup].result = visibleLightCount;
	}
}