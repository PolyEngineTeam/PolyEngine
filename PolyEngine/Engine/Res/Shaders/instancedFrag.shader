#version 330 core

uniform sampler2D i_color;

in vec3 vTexCoord;
out vec4 color;

void main()
{
    // vec2 uv = 2.0 * (vTexCoord - 0.5);
    color = vec4(vTexCoord.rgb, 1.0);
}