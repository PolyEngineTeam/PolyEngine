#version 330 core

struct DiffuseLight
{
	vec4 Color;
	float Intensity;
};

struct DirectionalLight
{
	DiffuseLight Base;
	vec4 Direction;
};

struct PointLight
{
	DiffuseLight Base;
	vec4 Position;
	float Range;
};

struct SpotLight
{
	DiffuseLight Base;
	vec4 Position;
	vec4 Direction;
	float Range;
	float CutOff;
	float OuterCutOff;
};

struct Lighting
{
	vec3 Diffuse;
	vec3 Specular;
};

struct Material
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Shininess;
};

uniform sampler2D uTexture;
uniform float uTime;
uniform vec4 uCameraForward;
uniform vec4 uCameraPosition;

uniform Material uMaterial;

uniform DiffuseLight uAmbientLight;
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;
uniform PointLight uPointLight[8];
uniform int uPointLightCount;
uniform SpotLight uSpotLight[8];
uniform int uSpotLightCount;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

vec3 ambientLighting()
{
	return uMaterial.Ambient.rgb * uAmbientLight.Color.rgb * uAmbientLight.Intensity;
}

// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 N, in vec3 L, in vec3 LightColor)
{
	// calculation as for Lambertian reflection
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	return vec3(NdotL) * uMaterial.Diffuse.rgb * LightColor;
}

// returns intensity of specular reflection
vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V, in vec3 LightColor)
{
	vec3 H = normalize(L + V);
	// to avoid issues when dot(N, L) is -1 and ceil could result in 1.0 for negative values
	float nearlyOne = 0.9999;
	float specularTerm = ceil(nearlyOne*dot(N, L)) * pow(dot(N, H), uMaterial.Shininess);
	return specularTerm * uMaterial.Specular.rgb * LightColor;
}

Lighting directionalLighting(in DirectionalLight dirLight, in vec3 positionWS, in vec3 normalWS, in vec3 toCamera)
{
	Lighting OUT;

	vec3 L = normalize(-dirLight.Direction.xyz);
	vec3 V = normalize(toCamera);
	vec3 N = normalize(normalWS);

	vec3 lightColor = dirLight.Base.Color.rgb * dirLight.Base.Intensity;

	OUT.Diffuse = diffuseLighting(N, L, lightColor);
	OUT.Specular = specularLighting(N, L, V, lightColor);

	return OUT;
}


Lighting pointLighting(in PointLight pointLight, in vec3 positionWS, in vec3 normalWS, in vec3 toCamera)
{
	Lighting OUT;

	vec3 o_toLight = normalize(pointLight.Position.xyz - positionWS);

	vec3 L = normalize(o_toLight);
	vec3 V = normalize(toCamera);
	vec3 N = normalize(normalWS);

	float dist = distance(pointLight.Position.xyz, positionWS);
	float att = clamp(1.0 - dist*dist / (pointLight.Range*pointLight.Range), 0.0, 1.0);
	att *= att;

	vec3 lightColor = pointLight.Base.Color.rgb * pointLight.Base.Intensity;
	
	OUT.Diffuse = diffuseLighting(N, L, lightColor) * att;
	OUT.Specular = specularLighting(N, L, V, lightColor) * att;

	return OUT;
}

Lighting spotLighting(in SpotLight spotLight, in vec3 positionWS, in vec3 normalWS, in vec3 toCamera)
{
	Lighting OUT;
	OUT.Diffuse = vec3(0.0);
	OUT.Specular = vec3(0.0);

	vec3 S = normalize(-spotLight.Direction.xyz);
	vec3 L = normalize(spotLight.Position.xyz - positionWS);
	vec3 V = normalize(toCamera);
	vec3 N = normalize(normalWS);
	
	float dist = distance(spotLight.Position.xyz, positionWS);
	float att = clamp(1.0 - dist*dist / (spotLight.Range*spotLight.Range), 0.0, 1.0);
	att *= att;
	
	float theta = dot(L, S);
	float epsilon = spotLight.CutOff - spotLight.OuterCutOff;
	float intensity = smoothstep(0.0, 1.0, clamp((theta - spotLight.OuterCutOff) / epsilon, 0.0, 1.0));

	vec3 lightColor = spotLight.Base.Color.rgb * spotLight.Base.Intensity *intensity;
	
	// get Blinn - Phong reflectance components
	OUT.Diffuse = diffuseLighting(N, L, lightColor) * att;
	OUT.Specular = specularLighting(N, L, V, lightColor) * att;

	return OUT;
}

void main() {

	vec4 texDiffuse = texture(uTexture, vTexCoord);

	if (texDiffuse.a < 0.1)
		discard;

	vec3 normalWS = normalize(vNormal);
	vec3 positionWS = vVertexPos;
	
	vec3 Iamb = ambientLighting();
	vec3 Idif =	vec3(0.0);
	vec3 Ispe =	vec3(0.0);

	vec3 toCamera = normalize(uCameraPosition.xyz - positionWS);

	for (int i = 0; i < uDirectionalLightCount; ++i)
	{
		Lighting lighting = directionalLighting(uDirectionalLight[i], positionWS, normalWS, toCamera);
		Idif += lighting.Diffuse;
		Ispe += lighting.Specular;
	}
	
	for (int i = 0; i < uPointLightCount; ++i)
	{
		Lighting lighting = pointLighting(uPointLight[i], positionWS, normalWS, toCamera);
		Idif += lighting.Diffuse;
		Ispe += lighting.Specular;
	}

	for (int i = 0; i < uSpotLightCount; ++i)
	{
		Lighting lighting = spotLighting(uSpotLight[i], positionWS, normalWS, toCamera);
		Idif += lighting.Diffuse;
		Ispe += lighting.Specular;
	}

	color.xyz = texDiffuse.rgb * (Iamb + Idif + Ispe);
	color.w = 1.0;
}