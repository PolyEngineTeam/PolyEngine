#version 330 core

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

uniform mat4 uTransform;

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

void main() {
	// color = uMaterial.Diffuse;
	color = vec4(1.0, 0.0, 0.0, 0.5);
}

