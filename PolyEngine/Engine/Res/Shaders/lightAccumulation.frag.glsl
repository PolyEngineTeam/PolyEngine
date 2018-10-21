#version 430 core

#ifndef MAX_DIRLIGHT_COUNT
#define MAX_DIRLIGHT_COUNT 8
#endif

in VERTEX_OUT
{
	vec3 positionInWorld;
	vec2 uv;
	vec3 normalInModel;
	mat3 tangentFromWorld;
	vec3 viewPositionInTangent;
	vec3 fragmentPositionInTangent;
 	vec4 fragmentPositionInDirLight;
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

// in VERTEX_OUT fragment_in;

layout(std430, binding = 0) readonly buffer LightBuffer
{
	Light bLights[];
};

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer
{
    VisibleIndex bVisibleIndicies[];
};

struct Material
{
	vec4 Emissive;
	vec4 Albedo;
	float Roughness;
	float Metallic;
	float OpacityMaskThreshold;
};

const uint MAX_NUM_LIGHTS = 1024;

uniform sampler2D uBrdfLUT;
uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D uDirShadowMap;

uniform sampler2D uEmissiveMap;
uniform sampler2D uAlbedoMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uNormalMap;
uniform sampler2D uAmbientOcclusionMap;


uniform float uTime;
uniform Material uMaterial;

uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;

uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;

const float PI = 3.14159265359;

// 64 samples of poisson disk at:
// https://www.geeks3d.com/20100628/3d-programming-ready-to-use-64-sample-poisson-disc/
// 32 samples produce similar results on shadow PCF
uniform vec2 poissonDisk[32] = vec2[] (
	vec2(-0.613392,  0.617481),
	vec2( 0.170019, -0.040254),
	vec2(-0.299417,  0.791925),
	vec2( 0.645680,  0.493210),
	vec2(-0.651784,  0.717887),
	vec2( 0.421003,  0.027070),
	vec2(-0.817194, -0.271096),
	vec2(-0.705374, -0.668203),
	vec2( 0.977050, -0.108615),
	vec2( 0.063326,  0.142369),
	vec2( 0.203528,  0.214331),
	vec2(-0.667531,  0.326090),
	vec2(-0.098422, -0.295755),
	vec2(-0.885922,  0.215369),
	vec2( 0.566637,  0.605213),
	vec2( 0.039766, -0.396100),
	vec2( 0.751946,  0.453352),
	vec2( 0.078707, -0.715323),
	vec2(-0.075838, -0.529344),
	vec2( 0.724479, -0.580798),
	vec2( 0.222999, -0.215125),
	vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753),
	vec2( 0.354411, -0.887570),
	vec2( 0.175817,  0.382366),
	vec2( 0.487472, -0.063082),
	vec2(-0.084078,  0.898312),
	vec2( 0.488876, -0.783441),
	vec2( 0.470016,  0.217933),
	vec2(-0.696890, -0.549791),
	vec2(-0.149693,  0.605762),
	vec2( 0.034211,  0.979980)
);

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
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.001, 1.0), 5.0); // clamp to prevent NaNs
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.001, 1.0), 5.0); // clamp to prevent NaNs
}

float hash(vec4 seed)
{
    float dot_product = dot(seed, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float calcShadow(vec4 fragPosInDirLight, float NdotL)
{
	// perform perspective divide
    // vec3 projCoords = fragPosInDirLight.xyz / fragPosInDirLight.w;
    vec3 projCoords = fragPosInDirLight.xyz;
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 1.0;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    // float closestDepth = texture(uDirShadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;    
    // check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - NdotL), 0.005);
	
	float pcfDepth = texture(uDirShadowMap, projCoords.xy).r;
    // float shadow = 0.0;	
    // vec2 texelSize = 1.0 / textureSize(uDirShadowMap, 0);
    // for (int i = 0; i <= 5; ++i)
    // {
    //     int index = int(32.0 * hash(vec4(gl_FragCoord.xyy, i))) % 32;
    //     float pcfDepth = texture(uDirShadowMap, projCoords.xy + poissonDisk[index] * texelSize).r;
    //     shadow += currentDepth - bias > pcfDepth ? 0.0 : 0.2;
    // }

    return step(currentDepth - bias, pcfDepth);
	// return pcfDepth;
}

void main()
{
    float debugLightCount = 0.0;

	vec4 emissive = uMaterial.Emissive * texture(uEmissiveMap, fragment_in.uv);
	vec4 albedo = uMaterial.Albedo * texture(uAlbedoMap, fragment_in.uv);
	float roughness = uMaterial.Roughness * texture(uRoughnessMap, fragment_in.uv).r;
	float metallic = uMaterial.Metallic * texture(uMetallicMap, fragment_in.uv).r;
	vec3 normal = normalize(texture(uNormalMap, fragment_in.uv).rgb * 2.0 - 1.0);
	float ao = texture(uAmbientOcclusionMap, fragment_in.uv).r;

    albedo = clamp(albedo, vec4(0.0), vec4(1.0));
    roughness = clamp(roughness, 0.0, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    ao = clamp(ao, 0.0, 1.0);

	if (albedo.a < uMaterial.OpacityMaskThreshold)
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
	F0 = mix(F0, albedo.rgb, vec3(metallic));

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
		int lightIndex = bVisibleIndicies[TileOffset + i].Index;
		Light light = bLights[lightIndex];

		// light attenuation based on Real Shading in Unreal Engine 4:
		// http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
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

        vec3 nominator = clamp(NDF * G * F, 0.0, 1000.0); // clamp to prevent NaN from fireflies
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		
		if (bVisibleIndicies[TileOffset + i + 1].Index == -1)
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

        vec3 nominator = min(NDF * G * F, 1000.0); // clamp to prevent NaN from fireflies
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
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);
        
        float shadow = 1.0;
        if (i == 0)
        {
            shadow = calcShadow(fragment_in.fragmentPositionInDirLight, NdotL);
        }

		// add to outgoing radiance Lo
        Lo += shadow * (kD * albedo.rgb / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

	vec3 amF = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	// ambient lighting (note that the next IBL tutorial will replace 
	// this ambient lighting with environment lighting).
    vec3 amkS = amF;
	vec3 amkD = 1.0 - amkS;
	vec3 irradiance = texture(uIrradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo.rgb;

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(uBrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (amF * envBRDF.x + envBRDF.y);
    
	vec3 ambient = (amkD * diffuse + specular) * ao;
    oColor.rgb = ambient + emissive.rgb + Lo;

	oNormal.rgb = (WorldFromTangent * normal) * 0.5 + 0.5;
}
