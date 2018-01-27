#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aTexCoord;
layout(location = 3) in mat4 aOffset;

out vec3 vTexCoord;

uniform mat4 uMVP;
uniform float uTime;

float nrand(float n)
{
    return fract(sin(dot(n, 12.9898)) * 43758.5453);
}

void main()
{
   float rnd = nrand(float(gl_InstanceID));
    // float phase = abs(sin(rnd + 7.5 * uTime));
    // p = mix(p, aOffset * p, vec4(phase));
    vec4 p = vec4(aPos, 1.0);
    p.z += 2.0 * rnd;
    p = aOffset * p;
    p = uMVP * p;
    gl_Position = p;
    vTexCoord = aTexCoord;
}