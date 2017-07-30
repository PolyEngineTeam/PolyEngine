#version 330 core

uniform sampler2D ourTexture;
uniform vec4 uBaseColor;
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

uniform DiffuseLight uDiffuseLight;
uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight[8];
uniform int uPointLightCount;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;


// calculates ambient light
vec4 ambientlLight(DiffuseLight ambientLight) {
	return vec4(ambientLight.Color.rgb * max(ambientLight.Intensity, 0), 1);
}

// calculates diffuse factor of any light
vec4 diffuseLight(DiffuseLight base, vec3 direction, vec3 normal) {
	vec4 diffuseColor = vec4(0.0f,0.0f,0.0f,0.0f);	
	float diffuseFactor =  dot(normal,-direction);	
	
	diffuseColor = vec4(base.Color.rgb, 1.0) * max(base.Intensity, 0) * max(diffuseFactor, 0);
	return diffuseColor;
}

// calculates attenuation of the point light
float pointLightAttenuation(PointLight light, vec3 vertexPos) {
	float distanceToLight = length(light.Position.xyz - vertexPos);
	if( distanceToLight > light.Range ) {
		return 0.0f;
	}
    return 1.0 / (1.0 + light.Attenuation * pow(distanceToLight, 2));
}

void main() {
	vec4 texColor = vec4(vTexCoord, 0.0, 1.0); // texture(ourTexture, vTexCoord);

  if(texColor.a < 0.1)
         discard;

  texColor += uBaseColor;
  
  vec4 ambient = texColor * ambientlLight(uDiffuseLight);
  vec3 normal = normalize(transpose(inverse(mat3(uTransform))) * vNormal);
  
  vec4 directional = texColor * diffuseLight(uDirectionalLight.Base, uDirectionalLight.Direction.xyz, normal);
  
  for(int i=8; i<uPointLightCount; ++i)
  {
    vec3 lightPos = uPointLight[i].Position.xyz;
    directional += texColor * diffuseLight(uPointLight[i].Base,  normalize(lightPos - vVertexPos), normal) 
      * pointLightAttenuation(uPointLight[i], vVertexPos);
  }
  
  color = ambient + directional;
  vec2 p = (vTexCoord - 0.5)*2.0;
  float a = clamp(1.0 - length(p), 0.0, 1.0);
  a = smoothstep(0.25, 1.0, a);

  color = 0.7*vec4(0.0, 1.0, 0.0, a);
}