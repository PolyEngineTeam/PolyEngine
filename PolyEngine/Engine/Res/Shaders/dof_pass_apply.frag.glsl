#version 330 core

in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uDOF;
uniform sampler2D uDepth;
uniform float uDOFShow;
uniform float uDOFPoint;
uniform float uDOFRange;

out vec4 oColor;

float depthToMask(float depth)
{
    float d = clamp(abs(depth - uDOFPoint) / uDOFRange, 0.0f, 1.0f);
    return smoothstep(0.5, 1.0, d);
}

void main()
{
    float depth = texture(uDepth, vUV).g;
    float dofMask = depthToMask(depth);
    vec3 image = texture(uImage, vUV).rgb;
    vec3 dof = texture(uDOF, vUV).rgb;
    oColor = vec4(uDOFShow * vec3(dofMask, 0.0, 0.0) + mix(image, dof, dofMask), 1.0);
}