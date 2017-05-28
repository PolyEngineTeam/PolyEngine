#version 330 core

layout (location = 0) in vec4 a_vertexPos;
layout (location = 1) in vec2 a_texCoords;

out vec2 v_texCoords;

uniform mat4 u_projection;
uniform vec4 u_position;

void main()
{
    gl_Position = u_projection * (u_position + a_vertexPos);
    v_texCoords = a_texCoords;
}  