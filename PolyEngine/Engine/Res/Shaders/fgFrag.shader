#version 330 core

uniform sampler2D i_color;
uniform float uTime;
uniform vec2 uResolution;

in vec2 vTexCoord;
out vec4 color;

#define CASHETES 1

// [1000.0, 40000.0]
// #define COLOR_TEMP_VALUE 6500.0 cold
#define COLOR_TEMP_VALUE 20000.0
#define COLOR_TEMP_POWER 1.0
#define ABERATION_POWER 0.55
#define GRAIN_POWER 0.3
#define STRIPES_POWER 0.0
#define VIGNETE_POWER 0.60


// by hornet
// https://www.shadertoy.com/view/XssGz8
// To the extent possible under law,
// the author has waived all copyright and related or neighboring rights to this work.

#define LUMINANCE_PRESERVATION 0.75

#define EPSILON 1e-10

float saturate(float v) { return clamp(v, 0.0, 1.0); }
vec2  saturate(vec2  v) { return clamp(v, vec2(0.0), vec2(1.0)); }
vec3  saturate(vec3  v) { return clamp(v, vec3(0.0), vec3(1.0)); }
vec4  saturate(vec4  v) { return clamp(v, vec4(0.0), vec4(1.0)); }

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

float remap(float t, float a, float b) {
	return clamp((t - a) / (b - a), 0.0, 1.0);
}
vec2 remap(vec2 t, vec2 a, vec2 b) {
	return clamp((t - a) / (b - a), 0.0, 1.0);
}

//note: input [0;1]
vec3 spectrum_offset_rgb(float t)
{
	//note: optimisation from https://twitter.com/Stubbesaurus/status/818847844790575104
	//t = 3.0 * t - 0.5;
	//vec3 ret = clamp( vec3(1.0-t, 1.0-abs(t-1.0), t-1.0), 0.0, 1.0);
	float t0 = 3.0 * t - 1.5;
	vec3 ret = clamp(vec3(-t0, 1.0 - abs(t0), t0), 0.0, 1.0);

	//note: old crappy code
	//vec3 ret;
	//float lo = step(t,0.5);
	//float hi = 1.0-lo;
	//float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );
	//ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);

	return ret;
	//return smoothstep( vec3(0.0), vec3(1.0), ret );
	//return pow( ret, vec3(1.0/2.2) );

}

const float gamma = 2.2;
vec3 lin2srgb(vec3 c)
{
	return pow(c, vec3(gamma));
}
vec3 srgb2lin(vec3 c)
{
	return pow(c, vec3(1.0 / gamma));
}


vec3 yCgCo2rgb(vec3 ycc)
{
	float R = ycc.x - ycc.y + ycc.z;
	float G = ycc.x + ycc.y;
	float B = ycc.x - ycc.y - ycc.z;
	return vec3(R, G, B);
}

vec3 spectrum_offset_ycgco(float t)
{
	//vec3 ygo = vec3( 1.0, 1.5*t, 0.0 ); //green-pink
	//vec3 ygo = vec3( 1.0, -1.5*t, 0.0 ); //green-purple
	vec3 ygo = vec3(1.0, 0.0, -1.25*t); //cyan-orange
										//vec3 ygo = vec3( 1.0, 0.0, 1.5*t ); //brownyello-blue
	return yCgCo2rgb(ygo);
}

vec3 yuv2rgb(vec3 yuv)
{
	vec3 rgb;
	rgb.r = yuv.x + yuv.z * 1.13983;
	rgb.g = yuv.x + dot(vec2(-0.39465, -0.58060), yuv.yz);
	rgb.b = yuv.x + yuv.y * 2.03211;
	return rgb;
}


// ====

//note: from https://www.shadertoy.com/view/XslGz8
vec2 radialdistort(vec2 coord, vec2 amt)
{
	vec2 cc = coord - 0.5;
	return coord + 2.0 * cc * amt;
}

// Given a vec2 in [-1,+1], generate a texture coord in [0,+1]
vec2 barrelDistortion(vec2 p, vec2 amt)
{
	p = 2.0 * p - 1.0;

	/*
	const float maxBarrelPower = 5.0;
	//note: http://glsl.heroku.com/e#3290.7 , copied from Little Grasshopper
	float theta  = atan(p.y, p.x);
	vec2 radius = vec2( length(p) );
	radius = pow(radius, 1.0 + maxBarrelPower * amt);
	p.x = radius.x * cos(theta);
	p.y = radius.y * sin(theta);

	/*/
	// much faster version
	//const float maxBarrelPower = 5.0;
	//float radius = length(p);
	float maxBarrelPower = sqrt(5.0);
	float radius = dot(p, p); //faster but doesn't match above accurately
	p *= pow(vec2(radius), maxBarrelPower * amt);
	/* */

	return p * 0.5 + 0.5;
}

//note: from https://www.shadertoy.com/view/MlSXR3
vec2 brownConradyDistortion(vec2 uv, float dist)
{
	uv = uv * 2.0 - 1.0;
	// positive values of K1 give barrel distortion, negative give pincushion
	float barrelDistortion1 = 0.1 * dist; // K1 in text books
	float barrelDistortion2 = -0.025 * dist; // K2 in text books

	float r2 = dot(uv, uv);
	uv *= 1.0 + barrelDistortion1 * r2 + barrelDistortion2 * r2 * r2;
	//uv *= 1.0 + barrelDistortion1 * r2;

	// tangential distortion (due to off center lens elements)
	// is not modeled in this function, but if it was, the terms would go here
	return uv * 0.5 + 0.5;
}

vec2 distort(vec2 uv, float t, vec2 min_distort, vec2 max_distort)
{
	vec2 dist = mix(min_distort, max_distort, t);
	//return radialdistort( uv, 2.0 * dist );
	//return barrelDistortion( uv, 1.75 * dist ); //distortion at center
	return brownConradyDistortion(uv, 75.0 * dist.x);
}

// ====

vec3 spectrum_offset_yuv(float t)
{
	//vec3 yuv = vec3( 1.0, 3.0*t, 0.0 ); //purple-green
	//vec3 yuv = vec3( 1.0, 0.0, 2.0*t ); //purple-green
	vec3 yuv = vec3(1.0, 0.0, -1.0*t); //cyan-orange
									   //vec3 yuv = vec3( 1.0, -0.75*t, 0.0 ); //brownyello-blue
	return yuv2rgb(yuv);
}

vec3 spectrum_offset(float t)
{
	return spectrum_offset_rgb(t);
	//return srgb2lin( spectrum_offset_rgb( t ) );
	//return lin2srgb( spectrum_offset_rgb( t ) );

	//return spectrum_offset_ycgco( t );
	//return spectrum_offset_yuv( t );
}

// ====

float nrand(vec2 n)
{
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

vec3 render(vec2 uv)
{
// #define DEBUG
#if defined( DEBUG )
	if (uv.x > 0.7 && uv.y > 0.7)
	{
		float d = length(vec2(0.77) - uv);
		d = min(d, length(vec2(0.82) - uv));
		d = min(d, length(vec2(0.875) - uv));
		return vec3(step(d, 0.025));
	}
#endif

	return srgb2lin(texture(i_color, uv).rgb);
}

vec3 Tonemap_ACES(const vec3 x) {
	// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

float stripes(vec2 uv)
{
	return mix(0.9, 1.0, floor(0.5 + fract(uv.y*0.25*uResolution.y)));
}

float grain(vec2 uv)
{
	float strength = 16.0;
	float x = (uv.x + 4.0) * (uv.y + 4.0) * (uTime * 10.0);
	vec4 grain = vec4(mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01) - 0.005) * strength;
	return 1.0 - grain;
}

float vinette(vec2 uv)
{
	float dist = length((vTexCoord - vec2(0.5, 0.5)) * 1.43f);
	float vinette = 1.0 - dist * dist;
	return mix(0.2, 1.0, vinette);
}

vec3 colorTemperature(vec3 color, float temperature)
{
	vec3 colorTempRGB = ColorTemperatureToRGB(temperature);
	float originalLuminance = Luminance(color);
	vec3 blended = mix(color, color * colorTempRGB, COLOR_TEMP_POWER);
	vec3 resultHSL = RGBtoHSL(blended);
	vec3 luminancePreservedRGB = HSLtoRGB(vec3(resultHSL.x, resultHSL.y, originalLuminance));
	return mix(blended, luminancePreservedRGB, LUMINANCE_PRESERVATION);
}

void main()
{
	vec2 uv = vTexCoord;


#if CASHETES
	vec2 p = -1.0 + 2.0 * vTexCoord.xy;
	p.x *= uResolution.x / uResolution.y;
	float cashetes = step(abs(p.y)*2.39, uResolution.x / uResolution.y);
	if (cashetes<0.1) {
		color = vec4(0.0);
		return;
	}
#endif

	float power = smoothstep(-0.2, 0.2, -cos(0.5*uTime));

	const float MAX_DIST_PX = 50.0;
	float max_distort_px = MAX_DIST_PX * (power * ABERATION_POWER);
	vec2 max_distort = vec2(max_distort_px) / uResolution.xy;
	vec2 min_distort = 0.5 * max_distort;

	//vec2 oversiz = vec2(1.0);
	vec2 oversiz = distort(vec2(1.0), 1.0, min_distort, max_distort);
	uv = remap(uv, 1.0 - oversiz, oversiz);

	//debug oversiz
	//vec2 distuv = distort( uv, 1.0, max_distort );
	//if ( abs(distuv.x-0.5)>0.5 || abs(distuv.y-0.5)>0.5)
	//{
	//    fragColor = vec4( 1.0, 0.0, 0.0, 1.0 ); return;
	//}
	
	// Chromatic Aberration
	const int num_iter = 7;
	const float stepsiz = 1.0 / (float(num_iter) - 1.0);
	float rnd = nrand(uv + fract(uTime));
	float t = rnd * stepsiz;
	

	vec3 sumcol = vec3(0.0);
	vec3 sumw = vec3(0.0);
	for (int i = 0; i<num_iter; ++i)
	{
		vec3 w = spectrum_offset(t);
		sumw += w;
		vec2 uvd = distort(uv, t, min_distort, max_distort); //TODO: move out of loop
		sumcol += w * render(uvd);
		t += stepsiz;
	}
	sumcol.rgb /= sumw;

	vec3 outcol = sumcol.rgb;
	outcol = lin2srgb(outcol);
	outcol += rnd / 255.0;
	
	color.rgb = outcol;

	// tonemapper
	color.rgb = Tonemap_ACES(color.rgb);

	// color temperature
	color.rgb = colorTemperature(color.rgb, mix(6000.0, COLOR_TEMP_VALUE, power));

	// Stripes
	color *= mix( 1.0, stripes(vTexCoord), STRIPES_POWER);

	// grain
	color *= mix( 1.0, grain(uv), GRAIN_POWER);

	// Vinette
	color *= mix(1.0, vinette(vTexCoord), VIGNETE_POWER);
}

/*
void main(){
  vec4 texColor = texture(i_color, vTexCoord);
  vec2 p = (vTexCoord - vec2(0.5)) * 2.0;
  p.x *= uResolution.x / uResolution.y;
  p.x += 0.1 * sin(uTime);
  color = texColor *smoothstep(0.1, 0.11, length(p));
}
*/