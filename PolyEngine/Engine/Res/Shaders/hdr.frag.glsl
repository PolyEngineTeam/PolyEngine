#version 430 core

in vec2 vUV;

uniform sampler2D uHdrBuffer;

uniform float uExposure;

out vec4 fragColor;

// Uses Reinhard tonemapping https://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf

// linear white point
const float W = 1.2;
const float T2 = 7.5;

float filmic_reinhard_curve(float x)
{
    float q = (T2 * T2 + 1.0) * x * x;
    return q / (q + x + T2 * T2);
}

vec3 filmic_reinhard(vec3 x)
{
    float w = filmic_reinhard_curve(W);
    return vec3(
        filmic_reinhard_curve(x.r),
        filmic_reinhard_curve(x.g),
        filmic_reinhard_curve(x.b)) / w;
}

void main()
{
    vec3 color = texture(uHdrBuffer, vUV).rgb;
    vec3 result = filmic_reinhard(uExposure * color);
    // vec3 result = vec3(1.0) - exp(-color * uExposure);
    // vec3 result = vec3(1.0) - exp(-color * 2.0);

    fragColor = vec4(result, 1.0);
}