// based on: http://mynameismjp.wordpress.com/2013/09/10/shadow-maps/
// based on: https://github.com/bartwronski/CSharpRenderer

#version 400 core

in vec2 vUV;

uniform sampler2D uEVSMap;
uniform int uIsHorizontal;

out vec4 oColor;
 
const float gaussianWeights[6] = float[] (
	0.12801535629105684,
	0.12299580237376932,
	0.10908749075572069,
	0.08931328345781858,
	0.06750152753344589,
	0.047094217733717074 
);

void main()
{
	vec4 sum = vec4(0.0);
		
	if(1 == uIsHorizontal)
	{
		sum += gaussianWeights[5] * textureLodOffset(uEVSMap, vUV, 0, ivec2(5, 0));
		sum += gaussianWeights[4] * textureLodOffset(uEVSMap, vUV, 0, ivec2(4, 0));
		sum += gaussianWeights[3] * textureLodOffset(uEVSMap, vUV, 0, ivec2(3, 0));
		sum += gaussianWeights[2] * textureLodOffset(uEVSMap, vUV, 0, ivec2(2, 0));
		sum += gaussianWeights[1] * textureLodOffset(uEVSMap, vUV, 0, ivec2(1, 0));
		sum += gaussianWeights[0] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 0));
		sum += gaussianWeights[1] * textureLodOffset(uEVSMap, vUV, 0, ivec2(1, 0));
		sum += gaussianWeights[2] * textureLodOffset(uEVSMap, vUV, 0, ivec2(2, 0));
		sum += gaussianWeights[3] * textureLodOffset(uEVSMap, vUV, 0, ivec2(3, 0));
		sum += gaussianWeights[4] * textureLodOffset(uEVSMap, vUV, 0, ivec2(4, 0));
		sum += gaussianWeights[5] * textureLodOffset(uEVSMap, vUV, 0, ivec2(5, 0));
	}
	else
	{
		sum += gaussianWeights[5] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 5));
		sum += gaussianWeights[4] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 4));
		sum += gaussianWeights[3] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 3));
		sum += gaussianWeights[2] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 2));
		sum += gaussianWeights[1] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 1));
		sum += gaussianWeights[0] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 0));
		sum += gaussianWeights[1] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 1));
		sum += gaussianWeights[2] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 2));
		sum += gaussianWeights[3] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 3));
		sum += gaussianWeights[4] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 4));
		sum += gaussianWeights[5] * textureLodOffset(uEVSMap, vUV, 0, ivec2(0, 5));
	}


	oColor = sum;
}