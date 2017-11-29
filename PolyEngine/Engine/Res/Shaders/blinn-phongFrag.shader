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
	float Attenuation;
	float Range;
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
uniform vec4 uCameraForward;
uniform vec4 uCameraPosition;

uniform Material uMaterial;

uniform DiffuseLight uAmbientLight;
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;
uniform PointLight uPointLight[8];
uniform int uPointLightCount;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

// float attenuate(vec3 lightDirection, float radius) {
// 	float cutoff = 0.5;
// 	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
// 	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
// 	attenuation = (attenuation - cutoff) / (1.0 - cutoff);
// 
// 	return clamp(attenuation, 0.0, 1.0);
// }

vec3 ambientLighting()
{
	return uMaterial.Ambient.rgb * uAmbientLight.Color.rgb * uAmbientLight.Intensity;
}

vec3 diffuseLighting(in vec3 N, in vec3 L)
{
	float diffuseTerm = max(dot(N, L), 0.0);
	return diffuseTerm * uMaterial.Diffuse.rgb;
}

vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V)
{
	vec3 H = normalize(L + V);
	float specularTerm = pow(max(dot(N, H), 0.0), uMaterial.Shininess);
	return specularTerm * uMaterial.Specular.rgb;
}

Lighting directionalLighting(in DirectionalLight dirLight, in vec3 positionWS, in vec3 normalWS, in vec3 toCamera)
{
	Lighting OUT;

	vec3 L = normalize(-dirLight.Direction.xyz);
	vec3 V = normalize(toCamera);
	vec3 N = normalize(normalWS);

	vec3 lightColor = dirLight.Base.Color.rgb * dirLight.Base.Intensity;

	OUT.Diffuse = diffuseLighting(N, L) * lightColor;	
	if (length(OUT.Diffuse) > 0.0)
	{		
		OUT.Specular = specularLighting(N, L, V) * lightColor;
	}

	return OUT;
}


Lighting pointLighting(in PointLight pointLight, in vec3 positionWS, in vec3 normalWS, in vec3 toCamera)
{
	Lighting OUT;

	vec3 L = normalize(pointLight.Position.xyz - positionWS);
	vec3 V = normalize(toCamera);
	vec3 N = normalize(normalWS);

	// float att = attenuate(L, pointLight.Range);
	float dist = distance(pointLight.Position.xyz, positionWS);
	float att = clamp(1.0 - dist*dist / (pointLight.Range*pointLight.Range), 0.0, 1.0);
	att = pow(att, 2.0);

	vec3 lightColor = pointLight.Base.Color.rgb * pointLight.Base.Intensity;
	
	OUT.Diffuse = diffuseLighting(N, L) * lightColor * att;
	if (length(OUT.Diffuse) > 0.0)
	{		
		OUT.Specular = specularLighting(N, L, V) * lightColor * att;
	}
	
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
	
	color.xyz = texDiffuse.rgb * (Iamb + Idif + Ispe);
	color.w = 1.0;
}