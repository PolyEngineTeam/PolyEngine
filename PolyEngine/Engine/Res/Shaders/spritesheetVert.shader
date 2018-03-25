#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

uniform mat4 uP;
uniform mat4 uMV;
uniform vec2 uScale;

void main()
{
    vec4 p = vec4(aPos, 1.0);
    p = uP * (uMV * vec4(0.0, 0.0, 0.0, 1.0)
        + vec4(p.x, p.y, 0.0, 0.0)
        * vec4(uScale, 1.0, 1.0));
    
    gl_Position = p;
    vUV = aUV;
}