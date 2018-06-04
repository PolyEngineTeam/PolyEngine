#version 430 core

#ifndef MAX_DIRLIGHT_COUNT
#define MAX_DIRLIGHT_COUNT 8
#endif

in VERTEX_OUT{
	vec3 positionInWorld;
	vec2 UV;
	vec3 normal;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} fragment_in;


struct DirectionalLight
{
    vec4 ColorIntensity;
    vec4 Direction;
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

struct Material
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Shininess;
};

uniform sampler2D uAlbedoMap;
uniform sampler2D uSpecularMap;
uniform sampler2D uNormalMap;

uniform Material uMaterial;

// uniform DirectionalLight uDirectionalLight[MAX_DIRLIGHT_COUNT];
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;

uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;

vec3 GetIrradiance(Light light, vec3 V, vec3 N, vec3 diffuse, float specular)
{
	// light faloff based on Real Shading in Unreal Engine 4 http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
	vec3 position = light.Position.xyz;
	float range = light.RangeIntensity.x;
    float dist = length(fragment_in.positionInWorld - position);
	float falloff = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (pow(dist, 2.0) + 1.0);
     
    vec3 tangentLightPosition = fragment_in.TBN * position;
    vec3 L = normalize(tangentLightPosition - fragment_in.tangentFragmentPosition);
    vec3 H = normalize(L + V);
	
    float NdotL = max(dot(L, N), 0.0);
    float NdotH = pow(max(dot(N, H), 0.0), uMaterial.Shininess);
	
	// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
    NdotH *= step(NdotL, 0.0);
	
    return light.Color.rgb * light.RangeIntensity.y * ((diffuse.rgb * uMaterial.Diffuse.rgb * NdotL) + (diffuse.rgb * uMaterial.Specular.rgb * specular * NdotH)) * falloff;
}

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = texture(uAlbedoMap, fragment_in.UV);
    vec3 normal = texture(uNormalMap, fragment_in.UV).rgb;
    float specular = texture(uSpecularMap, fragment_in.UV).r;
	// oColor.rgb = diffuse.rgb; return;
    // oColor.rgb = normal; return;
    // oColor.rgb = vec3(specular); return;

    normal = normalize(normal * 2.0 - 1.0);

    if (diffuse.a < 0.5)
    {
        discard;
    }

    vec3 V = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);
 
    color.rgb += uMaterial.Ambient.rgb;

    ivec2 WorkGroupSize = ivec2(16, 16);
    ivec2 NumWorkGroups = ivec2(uWorkGroupsX, uWorkGroupsY);
    ivec2 WorkGroupID = (ivec2(gl_FragCoord.xy) / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint TileOffset = IndexWorkGroup * MAX_NUM_LIGHTS;

    uint Count = uint(uLightCount);
    for (uint i = 0; i < Count; ++i)
	{
        int lightIndex = bVisibleLightIndicesBuffer.data[TileOffset + i].Index;
        Light light = bLightBuffer.data[lightIndex];
        color.rgb += GetIrradiance(light, V, normal, diffuse.rgb, specular);
        
		if (bVisibleLightIndicesBuffer.data[TileOffset + i + 1].Index == -1)
            break;
    }

    for (int i = 0; i < uDirectionalLightCount; ++i)
    {
		DirectionalLight dirLight = uDirectionalLight[i];
		vec3 tangentLightDir = fragment_in.TBN * dirLight.Direction.xyz;
		vec3 L = normalize(tangentLightDir);
		vec3 H = normalize(L + V);
		vec3 N = normal;
	
		float NdotL = max(dot(L, N), 0.0);
        float NdotH = pow(max(dot(N, H), 0.0), uMaterial.Shininess);
	
		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
        NdotH *= step(NdotL, 0.0);
	
        color.rgb += dirLight.ColorIntensity.rgb * dirLight.ColorIntensity.w * ((diffuse.rgb * uMaterial.Diffuse.rgb * NdotL) + (diffuse.rgb * uMaterial.Specular.rgb * specular * NdotH));
    }


    oColor = color;

    mat3 WorldFromTangent = inverse(fragment_in.TBN);
    oNormal.rgb = (WorldFromTangent * normal) * 0.5 + 0.5;
}