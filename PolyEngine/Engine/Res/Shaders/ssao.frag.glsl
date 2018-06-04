#version 430 core

in vec2 vUV;

uniform sampler2D uBeauty;
uniform sampler2D uNormal;
uniform sampler2D uDepth;

uniform float uFarClippingPlane;
uniform mat4 uViewFromWorld;

out vec4 oColor;

#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define COUNT 16
#define FARCLIP 10.0
#define BIAS 0.3

vec3 hash32(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yxz + 19.19);
    return fract((p3.xxy + p3.yzz) * p3.zyx);
}

void main()
{
    mat3 ViewFromWorld = mat3(uViewFromWorld);
    vec3 normalWS = texture(uNormal, vUV).rgb * 2.0 - 1.0;
    vec3 normal = ViewFromWorld * normalWS;
    float depth = texture(uDepth, vUV).r * uFarClippingPlane;
    float radius = 0.1;
    float scale = radius / depth;
    
    float ao = 0.0;
    for (int i = 0; i < COUNT; i++)
    {
        vec2 rndUV = 1000.0 * vUV + 23.71 * float(i);
        vec3 rnd = hash32(rndUV);
        vec3 randNor = rnd * 2.0 - 1.0;

        if (dot(normal, randNor) < 0.0)
            randNor *= -1.0;
        
        vec2 off = randNor.xy * scale;
        vec3 sampleNormal = ViewFromWorld * (texture(uNormal, vUV + off).rgb * 2.0 - 1.0);
        float sampleDepth = texture(uDepth, vUV + off).r;
        float depthDelta = depth - sampleDepth * uFarClippingPlane;
        
        vec3 sampleDir = vec3(randNor.xy * radius, depthDelta);
        float occ = max(0.0, dot(normalize(normal.xyz), normalize(sampleDir)) - BIAS) / (length(sampleDir) + 1.0);
        ao += 1.0 - occ;
    }
    ao /= float(COUNT);
    
    vec3 color = texture(uBeauty, vUV).rgb;
    // oColor = vec4(normal, 1.0);
    oColor = vec4(ao * color, 1.0);
}