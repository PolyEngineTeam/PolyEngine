#version 430 core

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

layout(std430, binding = 1) readonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

uniform float near;
uniform float far;
uniform int workGroupsX;
uniform int workGroupsY;
uniform sampler2D uTexture;
uniform mat4 ClipFromWorld;

in vec2 vUV;
out vec4 fragColor;

vec2 ScreenSize = vec2(800.0, 600.0);

// Need to linearize the depth because we are using the projection
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

void main()
{
	// Determine which tile this pixel belongs to
    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    
    float minDepth = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].input);
	float visibleLights = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].result);

	vec3 tex = texture(uTexture, vUV).rgb;
    float depth = LinearizeDepth(tex.r)/far;

    vec2 compTilePosSS = outputBuffer.data[IndexWorkGroup].tilePosSS.xy;

    vec4 LightInClip = ClipFromWorld * lightBuffer.data[0].Position;
    vec4 LightInScreen = ScreenFromClip(LightInClip);
    float lightRange = fract(0.01 * length(LightInScreen.xy - gl_FragCoord.xy));
    // tex = pow(tex, vec3(0.4545));
    // fragColor = vec4(vec3(depth), 1.0);
    // fragColor = vec4(vec2(location) / vec2(256.0, 192.0), 0.0, 1.0);
    // fragColor = vec4(vec2(WorkGroupID) / NumWorkGroups, 0.0, 1.0);
    // fragColor = vec4(vec3(minDepth), 1.0);
    // fragColor = vec4(depth, minDepth, maxDepth, 1.0);
    // fragColor = vec4(depth, minDepth, lightRange, 1.0);
    // fragColor = vec4(depth, minDepth, 0.0, 1.0);
    // fragColor = vec4(compTilePosSS/ScreenSize, depth, 1.0);
    // fragColor = vec4(compTilePosSS / ScreenSize, visibleLights, 1.0);
    // fragColor = vec4(depth, 0.0, visibleLights, 1.0);
	fragColor = vec4(depth, minDepth, visibleLights, 1.0);
}