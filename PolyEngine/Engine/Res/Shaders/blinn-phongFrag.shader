#version 330 core

uniform sampler2D uTexture;
uniform vec4 uBaseColor;
uniform float uSpecularStrength;

uniform vec4 uCameraDirection;
uniform mat4 uTransform;

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

struct Phong
{
	vec3 Diffuse;
	vec3 Specular;
};

uniform DiffuseLight uAmbientLight;
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;
uniform PointLight uPointLight[8];
uniform int uPointLightCount;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

vec3 ambientlLight(DiffuseLight ambientLight) {
	return vec3(ambientLight.Color.rgb * max(ambientLight.Intensity, 0.0));
}

Phong directionalLight(DirectionalLight light, vec3 positionWS, vec3 normalWS) {
	Phong result;
	DiffuseLight base = light.Base;
	vec3 dir = light.Direction.xyz;
	float dirNdotL = max(dot(normalWS, dir), 0.0);
	result.Diffuse = vec3(base.Color.rgb) * max(base.Intensity, 0.0) * dirNdotL;
	
	vec3 reflectDir = reflect(-dir, normalWS);
	float spec = pow(max(dot(uCameraDirection.xyz, reflectDir), 0.0), 32);
	result.Specular = uSpecularStrength * spec * base.Color.rgb;
	
	return result;	
}

float pointLightAttenuation(PointLight light, vec3 vertexPos) {
	float distanceToLight = length(light.Position.xyz - vertexPos);
	float attentuation = 1.0 / (1.0 + light.Attenuation * pow(distanceToLight, 2));
	return mix(0.0, attentuation, step(distanceToLight, light.Range));
}

Phong pointLight(PointLight pointLight, vec3 positionWS, vec3 normalWS) {	
	Phong result;
	float attentuation = pointLightAttenuation(pointLight, vVertexPos);
	vec3 pointLightPos = pointLight.Position.xyz;
	vec3 pointLightDir = normalize(pointLightPos - vVertexPos);
	float pointNdotL = max(dot(normalWS, pointLightDir), 0.0);
	vec3 point = vec3(pointLight.Base.Color.rgb) * max(pointLight.Base.Intensity, 0.0) * pointNdotL;
	result.Diffuse = point*attentuation;
	
	vec3 reflectDir = reflect(-pointLightDir, normalWS);
	float spec = pow(max(dot(uCameraDirection.xyz, reflectDir), 0.0), 32);
	result.Specular = uSpecularStrength * spec * pointLight.Base.Color.rgb;
	
	return result;
}

void main() {
	vec4 texDiffuse = texture(uTexture, vTexCoord);

	if (texDiffuse.a < 0.1)
		discard;

	vec3 normalWS = normalize(transpose(inverse(mat3(uTransform))) * vNormal);
	
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	diffuse += ambientlLight(uAmbientLight);
	
	for (int i = 0; i < uDirectionalLightCount; ++i)
	{
		Phong phong = directionalLight(uDirectionalLight[i], vVertexPos, normalWS);
		diffuse += phong.Diffuse;
		specular += phong.Specular;
	}
	
	for (int i = 0; i < uPointLightCount; ++i)
	{
		Phong phong = pointLight(uPointLight[i], vVertexPos, normalWS);
		diffuse += phong.Diffuse;
		specular += phong.Specular;
	}
	
	vec3 phong = texDiffuse.rgb * (diffuse + specular);
	
	color = vec4(phong, 1.0);
}