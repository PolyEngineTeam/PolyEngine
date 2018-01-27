#version 330 core

uniform sampler2D i_color;

in vec2 vTexCoord;
out vec4 color;

void main()
{
    vec2 c = 2.0 * (vTexCoord - 0.5);
    // color = vec4((1.0 - max(0.0, dot(c, c))));
    color = 0.5*vec4(1.0 - clamp(dot(c, c), 0.0, 1.0));
}