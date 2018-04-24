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
uniform float time;
uniform float near;
uniform float far;
uniform mat4 ViewFromWorld;
uniform mat4 ClipFromWorld;
uniform mat4 ClipFromView;
uniform int lightCount;
uniform int screenSizeX;
uniform int screenSizeY;
uniform int workGroupsX;
uniform int workGroupsY;

shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;

shared vec4 LightsPosInScreen[NUM_LIGHTS];
shared vec4 LightsBoundsTopRight[NUM_LIGHTS];
shared vec4 LightsBoundsBottomLeft[NUM_LIGHTS];

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

// return 1 if v inside the box, return 0 otherwise
float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight)
{
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;
}

float insideAABBAABB(vec2 aMin, vec2 aMax, vec2 bMin, vec2 bMax)
{
	// TODO: branchless like insideBox
    if (aMax.x < bMin.x || aMin.x > bMax.x) return 0.0;
    if (aMax.y < bMin.y || aMin.y > bMax.y) return 0.0;
    return 1.0;
}

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main()
{
	uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
	uint IndexGlobal = gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x) + gl_GlobalInvocationID.x;

	if (gl_LocalInvocationIndex == 0)
	{
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
		visibleLightCount = 0;

	// shared vec4 LightsPosInWorld[NUM_LIGHTS];
	// shared vec4 LightsBoundsTopLeft[NUM_LIGHTS];
	// shared vec4 LightsBoundsBottomRight[NUM_LIGHTS];
        float tmpRadius = 10.0;
		for (int i = 0; i < NUM_LIGHTS; ++i)
		{
			// TODO: works on the right side of the screen,
			//		because it misses corners that are squeezed on left side
			Light light = lightBuffer.data[i];
			vec4 lightInWorld = vec4(light.Position.xyz, 1.0);
			vec4 lightInClip = ClipFromWorld * lightInWorld;
			LightsPosInScreen[i] = ScreenFromClip(lightInClip);

            vec4 lightInView = ViewFromWorld * lightInWorld;
			
            vec4 topRightInView = vec4(lightInView.xyz + vec3(tmpRadius), 1.0);
			vec4 topRightInClip = ClipFromView * topRightInView;
			LightsBoundsTopRight[i] = ScreenFromClip(topRightInClip);

            vec4 bottomLeftInView = vec4(lightInView.xyz - vec3(tmpRadius), 1.0);
            vec4 bottomLeftInClip = ClipFromView * bottomLeftInView;
			LightsBoundsBottomLeft[i] = ScreenFromClip(bottomLeftInClip);
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
	 
	
	// float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight)
	// shared vec4 LightsPosInScreen[NUM_LIGHTS];
	// shared vec4 LightsBoundsTopRight[NUM_LIGHTS];
	// shared vec4 LightsBoundsBottomLeft[NUM_LIGHTS];
	vec2 lightPosInScreen = LightsPosInScreen[0].xy;
	vec2 topRightInScreen = LightsBoundsTopRight[0].xy;
	vec2 bottomLeftInScreen = LightsBoundsBottomLeft[0].xy;
	 
	// Step 2: One thread should calculate the frustum planes to be used for this tile
	if (gl_LocalInvocationIndex == 0)
	{
        
	// vec4 centerInWorld = vec4(0.0, 0.0, 0.0, 1.0);
	// vec4 centerInClip = ClipFromWorld * centerInWorld;
	// vec4 centerInScreen = ScreenFromClip(centerInClip);

	// float isLit = insideBox(vec2(pixelInScreenSpace), topRightInScreen, bottomLeftInScreen);
	// visibleLightCount = floatBitsToUint(isLit);

	//	vec2 centerInScreen = 0.5 * vec2(screenSizeX, screenSizeY);
	//	float isLit = insideBox(lightPosInScreen, centerInScreen - vec2(100.0, 100.0), centerInScreen + vec2(100.0, 100.0));
	//	float isInside = insideBox(pixelInScreenSpace, centerInScreen - vec2(100.0, 100.0), centerInScreen + vec2(100.0, 100.0));
	//	visibleLightCount = floatBitsToUint(isInside * isLit);

    // vec2 centerInScreen = 0.5 * vec2(screenSizeX, screenSizeY);
    vec2 tileInScreen = gl_WorkGroupID.xy * vec2(TILE_SIZE);
	// float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight)
    float isLit = insideBox(lightPosInScreen, tileInScreen, tileInScreen + vec2(TILE_SIZE));
    // float isLitMax = insideBox(topRightInScreen, tileInScreen, tileInScreen + vec2(TILE_SIZE));
    // float isLitMin = insideBox(bottomLeftInScreen, tileInScreen, tileInScreen + vec2(TILE_SIZE));
    // float insideAABBAABB(vec2 aMin, vec2 aMax, vec2 bMin, vec2 bMax)
    float isTileLit = insideAABBAABB(bottomLeftInScreen, topRightInScreen, tileInScreen, tileInScreen + vec2(TILE_SIZE));
    // float isInside = insideBox(pixelInScreenSpace, centerInScreen - vec2(100.0, 100.0), centerInScreen + vec2(100.0, 100.0));
    visibleLightCount = floatBitsToUint(isLit + isTileLit);

	// vec2 diff = abs(pixelInScreenSpace.xy - lightPosInScreen.xy);
	// outputBuffer.data[IndexWorkGroup].tilePosSS = vec4(diff, 0.0, 0.0);
	// if (length(diff) < 20.0)
	// {
	//     visibleLightCount = floatBitsToUint(1.0);
	// }
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