#version 330 core

in vec2 v_texCoords;

out vec4 color;

uniform sampler2D u_texSampler;
uniform vec4 u_textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texSampler, v_texCoords).r);
    color = u_textColor * sampled;
}  