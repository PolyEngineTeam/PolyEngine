// based on Goo by noby: https://www.shadertoy.com/view/lllBDM

#version 330 core
in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uSplashImage;
uniform vec4 uSplashTint;
uniform float uTime;
uniform vec4 uRes;
uniform vec4 uTint;
uniform float uGrainScale;
uniform float uVignetteScale;
uniform float uAbberationScale;
uniform float uGamma;

out vec4 oColor;

float hash(float c)
{
    return fract(sin(dot(c, 12.9898)) * 43758.5453);
}

const int N = 8;
vec3 ca(sampler2D t, vec2 UV, vec4 sampl, float scale)
{
    vec2 uv = 1.0 - 2.0 * UV;
    vec3 c = vec3(0);
    float rf = 1.0;
    float gf = 1.0;
    float bf = 1.0;
    float f = 1.0 / float(N);
    for (int i = 0; i < N; ++i)
    {
        c.r += f * texture(t, 0.5 - 0.5 * (uv * mix(1.0, rf, scale))).r;
        c.g += f * texture(t, 0.5 - 0.5 * (uv * mix(1.0, gf, scale))).g;
        c.b += f * texture(t, 0.5 - 0.5 * (uv * mix(1.0, bf, scale))).b;
        rf *= 0.9972;
        gf *= 0.998;
        bf /= 0.9988;
        c = clamp(c, 0.0, 1.0);
    }
    return c;
}

void main()
{
    vec2 p = 1. - 2. * vUV;
    p.y *= uRes.y / uRes.x;

    vec2 splashUV = vUV;
    splashUV *= 5.0;
    splashUV.y += 0.1;
    
    float maskX = step(splashUV.x, 1.0) * step(0.0, splashUV.x);
    float maskY = step(splashUV.y, 1.0) * step(0.0, splashUV.y);

    vec4 image = texture(uImage, vUV);
    vec4 splash = texture(uSplashImage, splashUV);
    // vec4 splash = texture(uSplashImage, vUV);
	vec3 color = ca(uImage, vUV, image, uAbberationScale);
    
	float vignette = 1.25 / (1.1 + 1.1 * dot(p, p));
    vignette *= vignette;
    vignette = mix(1.0, smoothstep(0.1, 1.1, vignette), 0.25);
    float noise = uGrainScale * .012 * vec3(hash(length(p) * uTime)).x;
    color = mix(color, color * vignette, uVignetteScale) + noise;

    vec3 result = pow(color.rgb * uTint.rgb, vec3(1.0 / uGamma));

    // result.rgb = vec3(splash.a);
    result = mix(result.rgb, uSplashTint.rgb, splash.r * maskX * maskY);
    oColor = vec4(result, 1.0);
}