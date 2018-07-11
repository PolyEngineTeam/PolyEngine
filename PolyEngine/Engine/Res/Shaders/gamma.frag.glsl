#version 430 core

in vec2 vUV;

uniform sampler2D uImage;

uniform float uGamma;

out vec4 oColor;

void main()
{
    vec3 color = texture(uImage, vUV).rgb;
    vec3 result = pow(color, vec3(1.0 / uGamma));
    oColor = vec4(result, 1.0);
}