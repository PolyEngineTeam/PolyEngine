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
	float specularTerm = 0;

	// calculate specular reflection only if
	// the surface is oriented to the light source
	if (dot(N, L) > 0)
	{
		// half vector
		vec3 H = normalize(L + V);
		specularTerm = pow(dot(N, H), uMaterial.Shininess);
	}
	return specularTerm * uMaterial.Specular.rgb * LightColor;
}

void main() {

	vec4 texDiffuse = texture(uTexture, vTexCoord);

	if (texDiffuse.a < 0.1)
		discard;

	vec3 normalWS = normalize(vNormal);
	vec3 positionWS = vVertexPos;
	
	vec3 Iamb = vec3(0.0);
	vec3 Idif =	vec3(0.0);
	vec3 Ispe =	vec3(0.0);

	for (int i = 0; i < uPointLightCount; ++i)
	{
		vec3 u_cameraPosition = uCameraPosition.xyz;
		vec3 u_lightPosition = uPointLight[i].Position.xyz;
		float u_lightRadius = uPointLight[i].Range;

		vec3 o_toLight = normalize(u_lightPosition - positionWS);
		vec3 o_toCamera = normalize(u_cameraPosition - positionWS);
		vec3 o_normal = normalWS;

		// normalize vectors after interpolation
		vec3 L = normalize(o_toLight);
		vec3 V = normalize(o_toCamera);
		vec3 N = normalize(o_normal);

		// attentuation
		float dist = distance(u_lightPosition, positionWS);
		float att = clamp(1.0 - dist*dist/(u_lightRadius*u_lightRadius), 0.0, 1.0);
		att *= att;

		vec3 lightColor = uPointLight[i].Base.Color.rgb * uPointLight[i].Base.Intensity;

		// get Blinn-Phong reflectance components
		Iamb += ambientLighting();
		Idif += diffuseLighting(N, L, lightColor) * att;
		Ispe += specularLighting(N, L, V, lightColor) * att;
	}

	// combination of all components and diffuse color of the object
	color.xyz = texDiffuse.rgb * (Iamb + Idif + Ispe);

	// color.xyz = 0.5+0.5*normalWS; // need this as RenderingMode
	color.w = 1.0;
}