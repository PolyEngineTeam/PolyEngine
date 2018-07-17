#version 330 core

in vec2 vUV;

uniform sampler2D uImage;

out vec4 oColor;

void main()
{
    vec3 input = texture(uImage, vUV).rgb;
    float luminance = dot(input, vec3(0.2126, 0.7152, 0.0722));
    vec4 BrightColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(input, 1.0), step(1.0, luminance));
    oColor = BrightColor;
}