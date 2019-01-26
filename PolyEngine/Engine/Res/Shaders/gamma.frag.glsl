// based on Goo by noby: https://www.shadertoy.com/view/lllBDM

#version 330 core
in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uDepth;
uniform sampler2D uSplashImage;
uniform vec4 uSplashTint;
uniform float uTime;
uniform vec4 uRes;
uniform vec4 uTint;
uniform vec4 uFogColor;
uniform float uFogDensity;
uniform float uPlaneFar;
uniform float uPlaneNear;
uniform float uGrainScale;
uniform float uVignetteScale;
uniform float uAbberationScale;
uniform float uTemperature;
uniform float uGamma;

out vec4 oColor;

// playing with this value tweaks how dim or bright the resulting image is
#define LUMINANCE_PRESERVATION 0.75

#define EPSILON 1e-10

float saturate(float v)
{
    return clamp(v, 0.0, 1.0);
}
vec2 saturate(vec2 v)
{
    return clamp(v, vec2(0.0), vec2(1.0));
}
vec3 saturate(vec3 v)
{
    return clamp(v, vec3(0.0), vec3(1.0));
}
vec4 saturate(vec4 v)
{
    return clamp(v, vec4(0.0), vec4(1.0));
}

vec3 ColorTemperatureToRGB(float temperatureInKelvins)
{
    vec3 retColor;
	
    temperatureInKelvins = clamp(temperatureInKelvins, 1000.0, 40000.0) / 100.0;
    
    if (temperatureInKelvins <= 66.0)
    {
        retColor.r = 1.0;
        retColor.g = saturate(0.39008157876901960784 * log(temperatureInKelvins) - 0.63184144378862745098);
    }
    else
    {
        float t = temperatureInKelvins - 60.0;
        retColor.r = saturate(1.29293618606274509804 * pow(t, -0.1332047592));
        retColor.g = saturate(1.12989086089529411765 * pow(t, -0.0755148492));
    }
    
    if (temperatureInKelvins >= 66.0)
        retColor.b = 1.0;
    else if (temperatureInKelvins <= 19.0)
        retColor.b = 0.0;
    else
        retColor.b = saturate(0.54320678911019607843 * log(temperatureInKelvins - 10.0) - 1.19625408914);

    return retColor;
}

float Luminance(vec3 color)
{
    float fmin = min(min(color.r, color.g), color.b);
    float fmax = max(max(color.r, color.g), color.b);
    return (fmax + fmin) / 2.0;
}

vec3 HUEtoRGB(float H)
{
    float R = abs(H * 6.0 - 3.0) - 1.0;
    float G = 2.0 - abs(H * 6.0 - 2.0);
    float B = 2.0 - abs(H * 6.0 - 4.0);
    return saturate(vec3(R, G, B));
}

vec3 HSLtoRGB(in vec3 HSL)
{
    vec3 RGB = HUEtoRGB(HSL.x);
    float C = (1.0 - abs(2.0 * HSL.z - 1.0)) * HSL.y;
    return (RGB - 0.5) * C + vec3(HSL.z);
}
 
vec3 RGBtoHCV(vec3 RGB)
{
    // Based on work by Sam Hocevar and Emil Persson
    vec4 P = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0 / 3.0) : vec4(RGB.gb, 0.0, -1.0 / 3.0);
    vec4 Q = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6.0 * C + EPSILON) + Q.z);
    return vec3(H, C, Q.x);
}

vec3 RGBtoHSL(vec3 RGB)
{
    vec3 HCV = RGBtoHCV(RGB);
    float L = HCV.z - HCV.y * 0.5;
    float S = HCV.y / (1.0 - abs(L * 2.0 - 1.0) + EPSILON);
    return vec3(HCV.x, S, L);
}

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

	// exponential fog
    float fogDist = (texture(uDepth, vUV).g - uPlaneNear) / (uPlaneFar - uPlaneNear);
    float fogFalloff = 1.0 / exp(uFogDensity * fogDist);
	color = mix(uFogColor.rgb, color, clamp(fogFalloff, 0.0, 1.0));

	float vignette = 1.25 / (1.1 + 1.1 * dot(p, p));
    vignette *= vignette;
    vignette = mix(1.0, smoothstep(0.1, 1.1, vignette), 0.25);
    float noise = uGrainScale * .012 * vec3(hash(length(p) * uTime)).x;
    color = mix(color, color * vignette, uVignetteScale) + noise;

    vec3 result = pow(color.rgb * uTint.rgb, vec3(1.0 / uGamma));

    // result.rgb = vec3(splash.a);
    result = mix(result.rgb, uSplashTint.rgb, splash.r * maskX * maskY);

    vec3 blended = result.rgb * ColorTemperatureToRGB(12500.0);
    vec3 resultHSL = RGBtoHSL(blended);
    float originalLuminance = Luminance(result.rgb);
    vec3 luminancePreservedRGB = HSLtoRGB(vec3(resultHSL.x, resultHSL.y, originalLuminance));
	
    oColor = vec4(luminancePreservedRGB, 1.0);
}