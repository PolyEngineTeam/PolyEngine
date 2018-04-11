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
    vec4 tilePosSS;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer OutputBuffer {
	Output data[];
} outputBuffer;


#define NUM_LIGHTS 10

// Uniforms
uniform sampler2D depthMap;
uniform float near;
uniform float far;
uniform mat4 ClipFromWorld;
uniform int lightCount;
uniform int screenSizeX;
uniform int screenSizeY;
uniform int workGroupsX;
uniform int workGroupsY;

shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;

shared vec4 LightsPosInScreen[NUM_LIGHTS];
shared vec4 LightsBoundsInScreen[NUM_LIGHTS];

vec2 ScreenSize = vec2(800.0, 600.0);

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec4 ScreenFromClip(vec4 posInClip)
{
    vec4 posInScreen = vec4(((posInClip.xyz / posInClip.w) + 1.0) * 0.5, 1.0);
    posInScreen.xy *= ScreenSize;
    return posInScreen;
}

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main() {

    uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    uint IndexGlobal = gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x) + gl_GlobalInvocationID.x;

	if (gl_LocalInvocationIndex == 0)
	{
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
        visibleLightCount = 0;

        // shared vec4 LightsPosInWorld[NUM_LIGHTS];
        // shared vec4 LightsBoundsInWorld[NUM_LIGHTS];
        for (int i = 0; i < NUM_LIGHTS; ++i)
        {
            Light light = lightBuffer.data[i];
            vec4 lightInWorld = vec4(light.Position.xyz, 1.0);
            vec4 lightInClip = ClipFromWorld * lightInWorld;
            LightsPosInScreen[i] = ScreenFromClip(lightInClip);
        }
        
    }

	barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
	float maxDepth, minDepth;
    ivec2 pixelInScreenSpace = ivec2(gl_GlobalInvocationID.xy);
    ivec2 screenSize = ivec2(screenSizeX, screenSizeY);
	vec2 uv = vec2(pixelInScreenSpace) / screenSize;
	float depth = texture(depthMap, uv).r;
    depth = LinearizeDepth(depth) / far;
    
	uint depthInt = floatBitsToUint(depth);

	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);
	
	barrier();
	 
	 
	// Step 2: One thread should calculate the frustum planes to be used for this tile
    if (gl_LocalInvocationIndex == 0)
    {
        vec4 lightPosInScreen = LightsPosInScreen[0];
		
		// outputBuffer.data[IndexWorkGroup].tilePosSS = vec4(pixelInScreenSpace, 0.0, 0.0);
        vec2 diff = abs(pixelInScreenSpace.xy - lightPosInScreen.xy);
        outputBuffer.data[IndexWorkGroup].tilePosSS = vec4(diff, 0.0, 0.0);

        if (length(diff) < 20.0)
        {
            visibleLightCount = floatBitsToUint(1.0);
        }
    }

    if (gl_LocalInvocationIndex == 0)
    {
        outputBuffer.data[IndexWorkGroup].indexLocal = 0;
        outputBuffer.data[IndexWorkGroup].indexWorkGroup = IndexWorkGroup;
        outputBuffer.data[IndexWorkGroup].indexGlobal = 0; // IndexGlobal;
        outputBuffer.data[IndexWorkGroup].input = minDepthInt;
        outputBuffer.data[IndexWorkGroup].result = visibleLightCount;
    }
}