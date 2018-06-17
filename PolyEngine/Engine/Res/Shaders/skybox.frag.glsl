#version 330 core

in vec3 vUV;

uniform samplerCube uCubemap;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

void main()
{
    vec3 envColor = texture(uCubemap, vUV).rgb;
    // envColor = envColor / (envColor / vec3(1.0));    // HDR -> LDR
    // envColor = pow(envColor, vec3(1.0, 2.2));        // linear -> gamma

    color = vec4(envColor, 1.0);
    normal = vec4(0.0);
}