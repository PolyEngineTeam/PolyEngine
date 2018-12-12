// Based on: http://mynameismjp.wordpress.com/2013/09/10/shadow-maps/

uniform sampler2D uDirEVSMap;
uniform float uPositiveExponent;
uniform float uNegativeExponent;
uniform float uEVSMBias;
uniform float uLightBleedingReduction;

vec2 WarpDepth(float depth)
{
	vec2 exponents = vec2(uPositiveExponent, uNegativeExponent);
	// Rescale depth into [-1, 1]
	depth = depth * 2.0f - 1.0f;
	float pos =  exp( exponents.x * depth);
	float neg = -exp(-exponents.y * depth);
	return vec2(pos, neg);
}

float Linstep(float a, float b, float v)
{
	return clamp((v - a) / (b - a), 0.0, 1.0);
}

// Reduces VSM light bleedning
float ReduceLightBleeding(float pMax, float amount)
{
  // Remove the [0, amount] tail and linearly rescale (amount, 1].
   return Linstep(amount, 1.0, pMax);
}

float ChebyshevUpperBound(vec2 moments, float mean, float minVariance, float lightBleedingReduction)
{
	// Compute variance
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, minVariance);

	// Compute probabilistic upper bound
	float d = mean - moments.x;
	float pMax = variance / (variance + (d * d));

	pMax = ReduceLightBleeding(pMax, lightBleedingReduction);

	// One-tailed Chebyshev
	return (mean <= moments.x ? 1.0 : pMax);
}

float calculateShadowEVSM4(vec4 moments, float smSpaceDepth)
{
	vec2 exponents = vec2(uPositiveExponent, uNegativeExponent);
	vec2 warpedDepth = WarpDepth(smSpaceDepth);

	// Derivative of warping at depth
	vec2 depthScale = uEVSMBias * 0.01 * exponents * warpedDepth;
	vec2 minVariance = depthScale * depthScale;

	float posContrib = ChebyshevUpperBound(moments.xz, warpedDepth.x, minVariance.x, uLightBleedingReduction);
	float negContrib = ChebyshevUpperBound(moments.yw, warpedDepth.y, minVariance.y, uLightBleedingReduction);
	return min(posContrib, negContrib);
}

float calculateShadowEVSM2(vec4 moments, float smSpaceDepth)
{
	vec2 exponents = vec2(uPositiveExponent, uNegativeExponent);
	vec2 warpedDepth = WarpDepth(smSpaceDepth);

	// Derivative of warping at depth
	vec2 depthScale = uEVSMBias * 0.01 * exponents * warpedDepth;
	vec2 minVariance = depthScale * depthScale;

	return ChebyshevUpperBound(moments.xy, warpedDepth.x, minVariance.x, uLightBleedingReduction);
}

float calculateShadowESM(vec4 moments, float smSpaceDepth)
{
	vec2 warpedDepth = WarpDepth(smSpaceDepth);
	return clamp(moments.x / warpedDepth.x, 0.0, 1.0);
}