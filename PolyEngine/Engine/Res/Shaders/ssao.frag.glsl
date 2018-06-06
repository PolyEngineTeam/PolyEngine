#version 430 core

#define COUNT 16

in vec2 vUV;

uniform sampler2D uBeauty;
uniform sampler2D uNormal;
uniform sampler2D uDepth;
uniform sampler2D uNoise;

uniform float uFarClippingPlane;
uniform float uBias;
uniform vec4 uScreenRes;
uniform mat4 uViewFromWorld;
uniform vec4 uKernel[COUNT];

out vec4 oColor;

// based on:
// - https://www.shadertoy.com/view/4ltSz2
// - http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
// - https://www.shadertoy.com/view/Ms23Wm 
// - UnityEngine Screen Space Ambient Occlusion
void main()
{
    vec3 noise = texture(uNoise, vUV * uScreenRes.xy * 0.25).rgb;
    // vec3 origin = vViewRay * texture(uTexLinearDepth, vTexcoord).r;

    oColor = vec4(noise, 1.0);
}