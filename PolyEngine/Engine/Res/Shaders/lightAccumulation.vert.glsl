#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in	vec2 aUV;
layout(location = 2) in	vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in vec4 aBoneWeight;
layout(location = 6) in ivec4 aBoneIds;

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
	
	float sum = aBoneWeight.x +  aBoneWeight.y +  aBoneWeight.z +  aBoneWeight.w;
	
	vec4 vertexInModel = vec4(aPosition.xyz, 1.0);
	
	vec4 pos1 = uBones[aBoneIds.x] * vertexInModel;
	vec4 pos2 = uBones[aBoneIds.y] * vertexInModel;
	vec4 pos3 = uBones[aBoneIds.z] * vertexInModel;
	vec4 pos4 = uBones[aBoneIds.w] * vertexInModel;
	
	vec4 vertexInAnimModel  = vec4(pos1.xyz, 1.0) * aBoneWeight.x/sum;
         vertexInAnimModel += vec4(pos2.xyz, 1.0) * aBoneWeight.y/sum;
         vertexInAnimModel += vec4(pos3.xyz, 1.0) * aBoneWeight.z/sum;
         vertexInAnimModel += vec4(pos4.xyz, 1.0) * aBoneWeight.w/sum;
	
	if(sum == 0.0f)
		vertexInAnimModel = vertexInModel;
	
	gl_Position = uClipFromModel * mix(vertexInModel, vertexInAnimModel, uHasBones);
	vertex_out.positionInWorld = (uWorldFromModel * vertexInAnimModel).xyz;

	if(sum == 0.0f)
		vertexInAnimModel = vertexInModel;
	
	if (uHasBones < 0.5)
	{
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
	}
	else
	{
		
		vec3 normalInModel1 = aBoneWeight.x/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.x] ))) * aNormal /*InModel*/);
		vec3 normalInModel2 = aBoneWeight.y/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.y] ))) * aNormal /*InModel*/);
		vec3 normalInModel3 = aBoneWeight.z/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.z] ))) * aNormal /*InModel*/);
		vec3 normalInModel4 = aBoneWeight.w/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.w] ))) * aNormal /*InModel*/);
		vertex_out.normalInModel = normalize( normalInModel1 + normalInModel2 + normalInModel3 + normalInModel4 );
		
		// mat3 transposedModelFromWorld = transpose(inverse(mat3(uWorldFromModel)));
		vec3 tangentInWorld0 = aBoneWeight.x/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.x] ))) * aTangent);
		vec3 tangentInWorld1 = aBoneWeight.y/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.y] ))) * aTangent);
		vec3 tangentInWorld2 = aBoneWeight.z/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.z] ))) * aTangent);
		vec3 tangentInWorld3 = aBoneWeight.w/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.w] ))) * aTangent);
		vec3 tangentInWorld = normalize( tangentInWorld0 + tangentInWorld1 + tangentInWorld2 + tangentInWorld3);
		
		vec3 bitangentInWorld0 = aBoneWeight.x/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.x] ))) * aBitangent);
		vec3 bitangentInWorld1 = aBoneWeight.y/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.y] ))) * aBitangent);
		vec3 bitangentInWorld2 = aBoneWeight.z/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.z] ))) * aBitangent);
		vec3 bitangentInWorld3 = aBoneWeight.w/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.w] ))) * aBitangent);
		vec3 bitangentInWorld = normalize( bitangentInWorld0 + bitangentInWorld1 + bitangentInWorld2 + bitangentInWorld3 );
				
		vec3 normalInWorld0 = aBoneWeight.x/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.x] ))) * aNormal);
		vec3 normalInWorld1 = aBoneWeight.y/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.y] ))) * aNormal);
		vec3 normalInWorld2 = aBoneWeight.z/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.z] ))) * aNormal);
		vec3 normalInWorld3 = aBoneWeight.w/sum * normalize( transpose(inverse(mat3( uWorldFromModel * uBones[aBoneIds.w] ))) * aNormal);
		vec3 normalInWorld = normalize(normalInWorld0 + normalInWorld1 + normalInWorld2 + normalInWorld3 );

		// For tangent space normal mapping
		mat3 tangentFromWorld = transpose(mat3(tangentInWorld, bitangentInWorld, normalInWorld));
		vertex_out.viewPositionInTangent = tangentFromWorld * uViewPosition.xyz;
		vertex_out.fragmentPositionInTangent = tangentFromWorld * vertex_out.positionInWorld;
		vertex_out.tangentFromWorld = tangentFromWorld;
	}	

    vertex_out.fragmentPositionInDirLight = uDirLightFromWorld * vec4(vertex_out.positionInWorld, 1.0);
}