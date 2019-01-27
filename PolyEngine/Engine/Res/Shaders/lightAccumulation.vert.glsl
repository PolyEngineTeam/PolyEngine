#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in	vec2 aUV;
layout(location = 2) in	vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 aBoneWeight;
layout (location = 6) in ivec4 aBoneIds;

const int MAX_BONES = 64;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;
uniform vec4 uViewPosition;
uniform mat4 uDirLightFromWorld;
uniform float uHasBones;

uniform float uTime;

uniform mat4 uBones[MAX_BONES];

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
	
	float sum =  aBoneWeight.x +  aBoneWeight.y +  aBoneWeight.z +  aBoneWeight.w;
	
	vec4 vertexInBone = vec4(aPosition.xyz, 1.0);
	
	vec4 pos1 = uBones[aBoneIds.x] * vertexInBone;
	vec4 pos2 = uBones[aBoneIds.y] * vertexInBone;
	vec4 pos3 = uBones[aBoneIds.z] * vertexInBone;
	vec4 pos4 = uBones[aBoneIds.w] * vertexInBone;
	
	vec4 vertInModel = vec4(pos1.xyz, 1.0) *  aBoneWeight.x/sum;				
        vertInModel += vec4(pos2.xyz, 1.0) * aBoneWeight.y/sum;
        vertInModel += vec4(pos3.xyz, 1.0) * aBoneWeight.z/sum;
        vertInModel += vec4(pos4.xyz, 1.0) * aBoneWeight.w/sum;
	
	vertInModel.w = 1.0f;
	
	if(sum == 0.0f)
		vertInModel = vertexInBone;
	
	gl_Position = uClipFromModel * mix(vertexInBone, vertInModel, uHasBones);
	// gl_Position = uClipFromModel * vertexInBone;
	vertex_out.positionInWorld = (uWorldFromModel * vertInModel).xyz;

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