#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in	vec2 aUV;
layout(location = 2) in	vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;
uniform vec4 uViewPosition;
uniform mat4 uDirLightFromWorld;

out VERTEX_OUT
{
	vec3 positionInWorld;
	vec2 uv;
	vec3 normalInModel;
	mat3 tangentFromWorld;
	vec3 viewPositionInTangent;
	vec3 fragmentPositionInTangent;
	vec4 fragmentPositionInDirLight;
} vertex_out;

void main()
{
	vertex_out.uv = aUV;

	vec4 vertexInModel = vec4(aPosition.xyz, 1.0);
	gl_Position = uClipFromModel * vertexInModel;
	vertex_out.positionInWorld = (uWorldFromModel * vertexInModel).xyz;

	mat3 transposedModelFromWorld = transpose(inverse(mat3(uWorldFromModel)));
	vertex_out.normalInModel = normalize(transposedModelFromWorld * aNormal /*InModel*/);

	vec3 tangentInWorld = normalize(transposedModelFromWorld * aTangent);
	vec3 bitangentInWorld = normalize(transposedModelFromWorld * aBitangent);
	vec3 normalInWorld = normalize(transposedModelFromWorld * aNormal);

	// For tangent space normal mapping
	mat3 tangentFromWorld = transpose(mat3(tangentInWorld, bitangentInWorld, normalInWorld));
	vertex_out.viewPositionInTangent = tangentFromWorld * uViewPosition.xyz;
	vertex_out.fragmentPositionInTangent = tangentFromWorld * vertex_out.positionInWorld;
	vertex_out.tangentFromWorld = tangentFromWorld;

    vertex_out.fragmentPositionInDirLight = uDirLightFromWorld * vec4(vertex_out.positionInWorld, 1.0);
}