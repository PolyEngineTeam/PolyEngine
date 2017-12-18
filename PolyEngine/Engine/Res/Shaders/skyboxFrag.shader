#version 330 core

in vec3 vUV;

out vec4 color;

uniform samplerCube uCubemap;

void main()
{
    color = vec4(texture(uCubemap, vUV).rgb, 1.0);
}