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

uniform float time;
uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int lightCount;
uniform int workGroupsX;
uniform int workGroupsY;
uniform mat4 uClipFromWorld;

in vec3 vVertexPos;
in vec2 vTexCoord;

layout(location = 0) out vec4 color;

float distToLight(Light light)
{
    float dist = 1.0;
    vec3 position = light.Position.xyz;
    float radius = light.Radius;
    if (radius > 0.0)
    {
        dist = min(length(vVertexPos - position) / abs(radius), 1.0);
    }
    return fract(radius*dist) * (1.0 - smoothstep(0.99, 1.0, dist));
}

void main()
{

    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
     
	float visibleLights = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].result);
    float visibleLightsNormalized = clamp(visibleLights / 10.0, 0.0, 1.0);
    visibleLights = clamp(visibleLights, 0.0, 1.0);
    vec3 tileCount = vec3(0.5) + mix(vec3(0.0, 0.0, 0.5), vec3(0.5, 0.0, 0.0), visibleLightsNormalized);
    vec3 tileColor = mix(vec3(0.5), tileCount, visibleLights);

    float dist = 0.5;
    uint count = uint(lightCount);
	for (uint i = 0; i < count; i++)
    {
        dist += distToLight(lightBuffer.data[i]);
    }

	color = vec4(fract(vVertexPos), 1.0);
    color.xyz *= vec3(clamp(dist, 0.0, 1.0));
    color.xyz *= tileColor;
}
