// based on: http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/

uniform sampler2D uDirShadowMap;
uniform float uShadowBias;

float texture2DCompare(sampler2D depthTex, vec2 uv, float compare)
{
	float depth = texture(depthTex, uv).r;
	return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depth, vec2 size, vec2 uv, float compare)
{
	vec2 texelSize = 1.0 / size;
	vec2 f = fract(uv * size + 0.5);
	vec2 centroidUV = floor(uv * size + 0.5) / size;

	float lb = texture2DCompare(depth, centroidUV + texelSize * vec2(0.0, 0.0), compare);
	float lt = texture2DCompare(depth, centroidUV + texelSize * vec2(0.0, 1.0), compare);
	float rb = texture2DCompare(depth, centroidUV + texelSize * vec2(1.0, 0.0), compare);
	float rt = texture2DCompare(depth, centroidUV + texelSize * vec2(1.0, 1.0), compare);
	float a = mix(lb, lt, f.y);
	float b = mix(rb, rt, f.y);
	float c = mix(a, b, f.x);
	return c;
}

float calculateShadowPCF(vec3 posInLight)
{
	float compare = posInLight.z - uShadowBias;
	vec2 uv = posInLight.xy;
	vec2 size = textureSize(uDirShadowMap, 0);
	
	float result = 0.0;
	for(int x =- 1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			vec2 off = vec2(x, y) / size;
			result += texture2DShadowLerp(uDirShadowMap, size, uv + off, compare);
		}
	}
	return result / 9.0;
}