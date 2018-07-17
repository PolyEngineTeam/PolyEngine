#version 330 core

in vec2 vUV;

uniform sampler2D uImage;
uniform int uHorizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 oColor;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uImage, 0); // gets size of single texel
    vec3 result = texture(uImage, vUV).rgb * weight[0]; // current fragment's contribution
    if(uHorizontal > 0)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uImage, vUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(uImage, vUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    oColor = vec4(result, 1.0);
}

//// 

// uniform bool horizontal;
/*
void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
*/