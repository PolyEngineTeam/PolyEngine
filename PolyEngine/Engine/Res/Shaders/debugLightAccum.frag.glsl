#version 430 core

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

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} bLightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} bVisibleLightIndicesBuffer;

const uint MAX_NUM_LIGHTS = 1024;

uniform sampler2D uTexture;
uniform float uTime;
uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;
uniform mat4 uClipFromWorld;

in vec3 vVertexPos;
in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

float distToLight(Light light)
{
    float dist = 1.0;
    vec3 position = light.Position.xyz;
    float radius = light.RangeIntensity.x;
    if (radius > 0.0)
    {
        dist = min(length(vVertexPos - position) / abs(radius), 1.0);
    }
    return fract(radius*dist) * (1.0 - smoothstep(0.99, 1.0, dist));
}

void main()
{
    ivec2 WorkGroupSize = ivec2(16,16);
    ivec2 NumWorkGroups = ivec2(uWorkGroupsX, uWorkGroupsY);
    ivec2 WorkGroupID = (ivec2(gl_FragCoord.xy) / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint offset = IndexWorkGroup * MAX_NUM_LIGHTS;

    float dist = 0.5;
    uint count = uint(uLightCount);
    for (uint i = 0; i < count; i++)
    {
        int lightIndex = bVisibleLightIndicesBuffer.data[offset + i].Index;
        if (lightIndex < 0)
            break;
        dist += 0.1*distToLight(bLightBuffer.data[lightIndex]);
    }
    dist = clamp(dist, 0.0, 1.0);

    uint i;
    for (i = 0; i < count && bVisibleLightIndicesBuffer.data[offset + i].Index != -1; i++);
    float ratio = float(i) / (0.1 * float(MAX_NUM_LIGHTS));
    ratio = clamp(ratio, 0.0, 1.0);

    oColor = vec4(fract(vVertexPos), 1.0);
    
    float t = smoothstep(0.5, 0.5, sin(uTime));
    oColor.xyz *= mix(ratio, dist, t);
}