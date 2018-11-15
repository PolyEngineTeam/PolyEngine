// Source of inspiration and code snippets: http://mynameismjp.wordpress.com/2013/09/10/shadow-maps/

#version 400 core

in vec2 vUV;

uniform sampler2D uDepth;
uniform float uNear;
uniform float uFar;

out vec4 oColor;

#pragma include "evsm.inc.glsl" 
 
void main()
{
    vec4 depth = textureGather(uDepth, vUV, 0);
	// depth = clamp(depth, vec4(0.0), vec4(1.0));
	
	vec2 warpedDepth[4];
    warpedDepth[0] = WarpDepth(depth.x);
    warpedDepth[1] = WarpDepth(depth.y);
    warpedDepth[2] = WarpDepth(depth.z);
    warpedDepth[3] = WarpDepth(depth.w);
     
    vec4 outputEVSM[4];
    outputEVSM[0] = vec4(warpedDepth[0], warpedDepth[0] * warpedDepth[0]);
    outputEVSM[1] = vec4(warpedDepth[1], warpedDepth[1] * warpedDepth[1]);
    outputEVSM[2] = vec4(warpedDepth[2], warpedDepth[2] * warpedDepth[2]);
    outputEVSM[3] = vec4(warpedDepth[3], warpedDepth[3] * warpedDepth[3]);
	
	vec4 finalOutput = outputEVSM[0] + outputEVSM[1] + outputEVSM[2] + outputEVSM[3];
    finalOutput *= 0.25f;
	
    // oColor = depth;
	// oColor = vec4(warpedDepth[0].x, warpedDepth[1].x, warpedDepth[2].x, warpedDepth[3].x);
	// oColor = fract(finalOutput);
	oColor = finalOutput;
}