#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 vUV;

uniform mat4 uMVP;

void main()
{
    vUV = normalize(aPos);
    vec4 pos = uMVP * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
}