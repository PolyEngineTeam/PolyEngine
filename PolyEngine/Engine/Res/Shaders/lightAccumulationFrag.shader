#version 430 core

struct Irradiance
{
    vec3 Diffuse;
    vec3 Specular;
};

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
uniform vec4 uViewPosition;
uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

in vec3 vVertexInWorld;
in vec2 vUV;
in vec3 vNormalInWorld;
in vec3 vToCamera;

layout(location = 0) out vec4 oColor;

Irradiance GetIrradiance(Light light)
{
	// light faloff based on Real Shading in Unreal Engine 4 http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
    
    Irradiance OUT;

    vec3 toCamera = normalize(uViewPosition.xyz - vVertexInWorld);
    
	vec3 position = light.Position.xyz;
	float range = light.RangeIntensity.x;
	float dist = length(vVertexInWorld - position);
    
    vec3 L = normalize(position.xyz - vVertexInWorld);
    vec3 V = normalize(toCamera);
    vec3 N = normalize(vNormalInWorld);

    vec3 lightColor = light.RangeIntensity.y* light.Color.rgb;
	float falloff = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (pow(dist, 2.0) + 1.0);
    
	// diffuse
    float NdotL = max(dot(N, L), 0.0);

	// specular
    vec3 H = normalize(L + V);
    float NdotH = pow(max(dot(N, H), 0.0), 16.0);
    
    OUT.Diffuse = falloff * NdotL * lightColor;
    OUT.Specular = falloff * NdotH * lightColor;

    return OUT;
}

void main()
{
    ivec2 WorkGroupSize = ivec2(16, 16);
    ivec2 NumWorkGroups = ivec2(uWorkGroupsX, uWorkGroupsY);
    ivec2 WorkGroupID = (ivec2(gl_FragCoord.xy) / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint TileOffset = IndexWorkGroup * MAX_NUM_LIGHTS;

    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 Iamb = vec3(0.0, 0.0, 0.01);
    vec3 Idif = vec3(0.0);
    vec3 Ispe = vec3(0.0);

    uint Count = uint(uLightCount);
    for (uint i = 0; i < Count; ++i)
	{
        int lightIndex = bVisibleLightIndicesBuffer.data[TileOffset + i].Index;
		Irradiance pointIrradiance = GetIrradiance(bLightBuffer.data[lightIndex]);
        Idif += pointIrradiance.Diffuse;
        Ispe += pointIrradiance.Specular;
        
		if (bVisibleLightIndicesBuffer.data[TileOffset + i + 1].Index == -1)
            break;
    }
    
	color.rgb = Iamb + Idif + Ispe;

    oColor = color;
}