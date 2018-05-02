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

uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

in vec3 vVertexPos;
in vec2 vTexCoord;

uniform sampler2D uTexture;

layout(location = 0) out vec4 oColor;

vec3 Lighting(Light light)
{
	// light faloff based on Real Shading in Unreal Engine 4 http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
    
	vec3 col = vec3(0.0);

    vec3 position = light.Position.xyz;
	float range = light.RangeIntensity.x;
	float dist = length(vVertexPos - position);
    if (dist < range)
    {
        float falloff = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (pow(dist, 2.0) + 1.0);
        col = falloff * light.Color.rgb * light.RangeIntensity.y;
    }
	
    return col;
}

void main()
{
    ivec2 WorkGroupSize = ivec2(16, 16);
    ivec2 NumWorkGroups = ivec2(uWorkGroupsX, uWorkGroupsY);
    ivec2 WorkGroupID = (ivec2(gl_FragCoord.xy) / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint TileOffset = IndexWorkGroup * MAX_NUM_LIGHTS;

    vec3 col = vec3(0.0);
    uint Count = uint(uLightCount);
    for (uint i = 0; i < Count; i++)
    {
        int lightIndex = bVisibleLightIndicesBuffer.data[TileOffset + i].Index;
        if (lightIndex < 0)
            break;
        col += Lighting(bLightBuffer.data[lightIndex]);
    }

    oColor = vec4(col, 1.0);
}