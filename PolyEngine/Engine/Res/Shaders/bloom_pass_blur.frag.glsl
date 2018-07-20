#version 330 core

in vec2 vUV;

uniform sampler2D uImage;
uniform float uIsHorizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 oColor;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uImage, 0); // gets size of single texel
    vec3 result = texture(uImage, vUV).rgb * weight[0]; // current fragment's contribution
    vec2 blurDir = mix(vec2(1.0, 0.0), vec2(0.0, 1.0), step(0.5, uIsHorizontal));
    for(int i = 1; i < 5; ++i)
    {
        result += texture(uImage, vUV + blurDir * tex_offset.x * i).rgb * weight[i];
        result += texture(uImage, vUV - blurDir * tex_offset.x * i).rgb * weight[i];
    }
    oColor = vec4(result, 1.0);
}