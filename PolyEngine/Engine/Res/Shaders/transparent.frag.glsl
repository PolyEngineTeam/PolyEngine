#version 430 core

#ifndef MAX_DIRLIGHT_COUNT
#define MAX_DIRLIGHT_COUNT 8
#endif

in VERTEX_OUT
{
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

uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

void main()
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 texDiffuse = texture(uAlbedoMap, fragment_in.UV);
	vec3 texNormal = texture(uNormalMap, fragment_in.UV).rgb;
	float texSpecular = texture(uSpecularMap, fragment_in.UV).r;
	texNormal = normalize(texNormal * 2.0 - 1.0);    

	vec3 V = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);
 
	col.rgb += uMaterial.Ambient.rgb;

	for (int i = 0; i < uDirectionalLightCount; ++i)
	{
		DirectionalLight dirLight = uDirectionalLight[i];
		vec3 tangentLightDir = fragment_in.TBN * dirLight.Direction.xyz;
		vec3 L = normalize(tangentLightDir);
		vec3 H = normalize(L + V);
		vec3 N = texNormal;
	
		float NdotL = max(dot(L, N), 0.0);
		float NdotH = pow(max(dot(N, H), 0.0), uMaterial.Shininess);
	
		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
		NdotH *= step(NdotL, 0.0);
	
		col.rgb += dirLight.ColorIntensity.rgb * dirLight.ColorIntensity.w * ((texDiffuse.rgb * uMaterial.Diffuse.rgb * NdotL) + (texDiffuse.rgb * uMaterial.Specular.rgb * texSpecular * NdotH));
	}

	color = vec4(col.rgb, texDiffuse.a * uMaterial.Diffuse.a);
	normal = vec4(0.0);
}