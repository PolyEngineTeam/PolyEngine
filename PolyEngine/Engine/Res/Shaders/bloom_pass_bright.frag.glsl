#version 330 core

in vec2 vUV;

uniform sampler2D uImage;

out vec4 oColor;

void main()
{
    vec3 input = texture(uImage, vUV).rgb;
    // check whether fragment output is higher than threshold, if so output as brightness color
    float luminance = dot(input, vec3(0.2126, 0.7152, 0.0722));
    vec4 BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (luminance > 1.0)
        BrightColor = vec4(input, 1.0);

    oColor = BrightColor;
}