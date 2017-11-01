#version 330 core

uniform sampler2D ourTexture;
uniform vec4 uBaseColor;
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

// uniform DiffuseLight uDiffuseLight;
uniform DirectionalLight uDirectionalLight[8];
uniform int uDirectionalLightCount;
uniform PointLight uPointLight[8];
uniform int uPointLightCount;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

// calculates ambient light
// vec4 ambientlLight(DiffuseLight ambientLight) {
// 	return vec4(ambientLight.Color.rgb * max(ambientLight.Intensity, 0.0), 1.0);
// }

// calculates diffuse factor of any light
vec3 directionalLight(DirectionalLight light, vec3 normalWS) {
	DiffuseLight base = light.Base;
	vec3 dir = light.Direction.xyz;
	float dirNdotL = max(dot(normalWS, dir), 0.0);
	vec3 dirDiffuse = vec3(base.Color.rgb) * max(base.Intensity, 0.0) * dirNdotL;
	return dirDiffuse;
}

// calculates attenuation of the point light
// float pointLightAttenuation(PointLight light, vec3 vertexPos) {
// 	float distanceToLight = length(light.Position.xyz - vertexPos);
// 	float attentuation = 1.0 / (1.0 + light.Attenuation * pow(distanceToLight, 2));
// 	return mix(0.0, attentuation, step(distanceToLight, light.Range));
// }

void main() {
	vec4 texColor = texture(ourTexture, vTexCoord);

	if (texColor.a < 0.1)
		discard;

	// vec4 ambient = texColor * ambientlLight(uDiffuseLight);
	vec3 normalWS = normalize(transpose(inverse(mat3(uTransform))) * vNormal);
	vec3 dirDiffuse = vec3(0.0);
	for (int i = 0; i < uDirectionalLightCount; ++i)
	{
		dirDiffuse += directionalLight(uDirectionalLight[i], normalWS);
	}

	// vec3 pointLightPos = uPointLight[0].Position.xyz;
	// vec3 pointLightDir = normalize(pointLightPos - vVertexPos);
	// float pointNdotL = max(dot(normalWS, pointLightDir), 0.0);
	
	// vec4 point = texColor * diffuseLight(uDirectionalLight.BasdirectionalLight(DirectionalLight light, vec3 normalWS) {e, uDirectionalLight.Direction.xyz, normalWS);

	// for (int i = 8; i < uPointLightCount; ++i)
	// {
	// 	vec3 lightPos = uPointLight[i].Position.xyz;
	// 	point += texColor * diffuseLight(uPointLight[i].Base, normalize(lightPos - vVertexPos), normalWS)
	// 		* pointLightAttenuation(uPointLight[i], vVertexPos);
	// }
	
	color = vec4(dirDiffuse, 1.0);
	// color = vec4(normalWS*0.5+0.5, 1.0);
	// color = vec4(max(dot(normalWS, uLightDirectional.Direction.xyz), 0.0));
}