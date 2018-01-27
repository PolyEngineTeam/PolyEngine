#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 uP;
uniform mat4 uMV;
uniform float uTime;
uniform vec2 uScale;
// uniform float uScaleX;
// uniform float uScaleY;

float nrand(float n)
{
    return fract(sin(dot(n, 12.9898)) * 43758.5453);
}

void main()
{
    float rnd = nrand(float(gl_InstanceID));
    // float _ScaleY = 0.5+smoothstep( -0.8, 0.8, sin(2.0*7.5*uTime) );
    // float _ScaleX = 0.5+smoothstep( -0.8, 0.8, sin(2.0*7.5*uTime) );
	// float _ScaleY = 10.0;
    // float _ScaleX = 10.0;

    vec4 p = vec4(aPos, 1.0);
    p = uP * (uMV * vec4(0.0, 0.0, 0.0, 1.0)
        + vec4(p.x, p.y, 0.0, 0.0)
        * vec4(uScale, 1.0, 1.0));
    
    gl_Position = p;
    vTexCoord = aTexCoord;
}