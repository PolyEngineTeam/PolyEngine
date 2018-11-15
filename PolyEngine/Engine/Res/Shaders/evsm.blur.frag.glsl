// Source of inspiration and code snippets: http://mynameismjp.wordpress.com/2013/09/10/shadow-maps/

#version 400 core

in vec2 vUV;

uniform sampler2D uEVSMap;
uniform int uIsHorizontal;

out vec4 oColor;
 
uniform const float gaussianWeights[6] = float[] (
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
        
    for (int x = -5; x < 6; ++x)
    {
        // unoptimal, could exploit bilinear filtering
        sum += gaussianWeights[abs(x)] * textureLodOffset(uEVSMap, vUV, 0,
			1 == uIsHorizontal ? ivec2(x, 0) : ivec2(0, x)
        );
    }
	
	// for (int x = -2; x <= 2; ++x)
    // {
    //     // unoptimal, could exploit bilinear filtering
    //     sum += 1.0/5.0 * textureLodOffset(uEVSMap, vUV, 0,
	// 		1 == uIsHorizontal ? ivec2(x, 0) : ivec2(0, x)
    //     );
    // }

	oColor = sum;
}