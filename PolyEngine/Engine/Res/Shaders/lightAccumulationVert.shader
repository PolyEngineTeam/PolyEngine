#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in	vec2 aTexCoord;
layout(location = 2) in	vec3 aNormal;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;

out vec3 vVertexPos;
out	vec2 vTexCoord;

void main()
{
    vec4 vertexInModel = vec4(aPos.xyz, 1.0);
    gl_Position = uClipFromModel * vertexInModel;
    vVertexPos = (uWorldFromModel * vertexInModel).xyz;
    vTexCoord = aTexCoord;
}