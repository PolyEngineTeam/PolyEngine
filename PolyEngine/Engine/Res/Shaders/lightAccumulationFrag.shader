#version 430 core

#ifndef MAX_DIRLIGHT_COUNT
#define MAX_DIRLIGHT_COUNT 8
#endif

in VERTEX_OUT{
    vec3 positionInWorld;
    vec2 uv;
    vec3 normalInModel;
    mat3 tangentFromWorld;
    vec3 viewPositionInTangent;
    vec3 fragmentPositionInTangent;
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
    vec4 Emissive;
    vec4 Albedo;
    float Roughness;
    float Metallic;
};

uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D uBrdfLUT;

uniform sampler2D uEmissiveMap;
uniform sampler2D uAlbedoMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uNormalMap;
uniform sampler2D uAmbientOcclusionMap;

uniform Material uMaterial;

// uniform DirectionalLight uDirectionalLight[MAX_DIRLIGHT_COUNT];
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;

// uniform mat4 uWorldFromModel;

uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 emissive = uMaterial.Emissive * texture(uEmissiveMap, fragment_in.uv);
    vec4 albedo = uMaterial.Albedo * texture(uAlbedoMap, fragment_in.uv);
    float roughness = uMaterial.Roughness * texture(uRoughnessMap, fragment_in.uv).r;
    float metallic = uMaterial.Metallic * texture(uMetallicMap, fragment_in.uv).r;
    vec3 normal = texture(uNormalMap, fragment_in.uv).rgb;
    float ao = texture(uAmbientOcclusionMap, fragment_in.uv).r;

	// oColor.rgb = diffuse.rgb; return;
    // oColor.rgb = normal; return;
    // oColor.rgb = vec3(specular); return;

    normal = normalize(normal * 2.0 - 1.0);

    if (albedo.a < 0.5)
    {
        discard;
    }
    
    mat3 WorldFromTangent = inverse(fragment_in.tangentFromWorld);

    vec3 N = normal;
    vec3 V = normalize(fragment_in.viewPositionInTangent - fragment_in.fragmentPositionInTangent);
    vec3 R = WorldFromTangent * reflect(-V, N);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.rgb, vec3(uMaterial.Metallic));

	// reflectance equation
	vec3 Lo = vec3(0.0);

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

		// light attenuation based on Real Shading in Unreal Engine 4 http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
		// calculate per-light radiance
		vec3 tangentLightPosition = fragment_in.tangentFromWorld * light.Position.xyz;
        vec3 L = normalize(tangentLightPosition - fragment_in.fragmentPositionInTangent);
		// vec3 L = normalize(light.Position.xyz - fragment_in.positionInWorld);
		vec3 H = normalize(V + L);		
		float range = light.RangeIntensity.x;
		float dist = length(light.Position.xyz - fragment_in.positionInWorld);
		float attenuation = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (pow(dist, 2.0) + 1.0);
		vec3 radiance = light.Color.rgb * light.RangeIntensity.y * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - uMaterial.Metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        
		if (bVisibleLightIndicesBuffer.data[TileOffset + i + 1].Index == -1)
            break;
    }

	for (int i = 0; i < uDirectionalLightCount; ++i)
	{
		DirectionalLight dirLight = uDirectionalLight[i];
		vec3 tangentLightDir = fragment_in.tangentFromWorld * dirLight.Direction.xyz;
		vec3 L = normalize(tangentLightDir);
		vec3 H = normalize(L + V);

        vec3 radiance = dirLight.ColorIntensity.rgb * dirLight.ColorIntensity.w;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - uMaterial.Metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}
    
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	// ambient lighting (note that the next IBL tutorial will replace 
	// this ambient lighting with environment lighting).
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(uIrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(uBrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;

    oColor.rgb = ambient + Lo;

    // Debug texture input
    // oColor.rgb = emissive.rgb;
    // oColor.rgb = albedo.rgb;
    // oColor.rgb = normal.rgb * 0.5 + 0.5;
    // oColor.rgb = (WorldFromTangent * normal.rgb) * 0.5 + 0.5;
    // oColor.rgb = vec3(roughness);
    // oColor.rgb = vec3(metallic);
    // oColor.rgb = vec3(ao);

    oNormal.rgb = (WorldFromTangent * normal) * 0.5 + 0.5;
}
