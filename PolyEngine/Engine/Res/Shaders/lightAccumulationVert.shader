#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in	vec2 aUV;
layout(location = 2) in	vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;

// out vec3 vVertexInWorld;
// out	vec2 vUV;
// out vec3 vNormalInWorld;

out VERTEX_OUT {
	vec3 positionInWorld;
	vec2 UV;
	vec3 normal;
	// mat3 TBN;
	// vec3 tangentViewPosition;
	// vec3 tangentFragmentPosition;
} vertex_out;

void main()
{
	vertex_out.UV = aUV;

	vec4 vertexInModel = vec4(aPosition.xyz, 1.0);
	gl_Position = uClipFromModel * vertexInModel;
	vertex_out.positionInWorld = (uWorldFromModel * vertexInModel).xyz;

	mat3 transposedModelFromWorld = transpose(inverse(mat3(uWorldFromModel)));
	vertex_out.normal = normalize(transposedModelFromWorld * aNormal /*InModel*/);
}