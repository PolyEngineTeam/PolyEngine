#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in	vec2 aTexCoord;
layout(location = 2) in	vec3 aNormal;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;

out vec3 vVertexInWorld;
out	vec2 vUV;
out vec3 vNormalInWorld;

void main()
{
    vUV = aTexCoord;

    vec4 vertexInModel = vec4(aPos.xyz, 1.0);
    gl_Position = uClipFromModel * vertexInModel;
    vVertexInWorld = (uWorldFromModel * vertexInModel).xyz;

    mat3 transposedModelFromWorld = transpose(inverse(mat3(uWorldFromModel)));
    vNormalInWorld = normalize(transposedModelFromWorld * aNormal /*InModel*/); 
}