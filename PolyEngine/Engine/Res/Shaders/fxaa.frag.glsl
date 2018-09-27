#version 400 core // for textureGather in fxaa.inc.glsl

#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_QUALITY__PRESET 20

#pragma include "fxaa.inc.glsl"

in vec2 vUV;

uniform sampler2D uImage;

out vec4 oColor;

void main()
{
	// see FXAA3 QUALITY - PC for documentation
    FxaaFloat4 fxaaConsolePosPos = vec4(0.0, 1.0, 1.0, 1.0);
	FxaaFloat2 fxaaQualityRcpFrame = 1.0 / textureSize(uImage, 0);
	FxaaFloat4 fxaaConsoleRcpFrameOpt = vec4(fxaaQualityRcpFrame, fxaaQualityRcpFrame);
	FxaaFloat4 fxaaConsoleRcpFrameOpt2 = fxaaConsoleRcpFrameOpt;
	FxaaFloat4 fxaaConsole360RcpFrameOpt2 = fxaaConsoleRcpFrameOpt2;
	FxaaFloat fxaaQualitySubpix = 0.75;
	FxaaFloat fxaaQualityEdgeThreshold = 0.166;
	FxaaFloat fxaaQualityEdgeThresholdMin = 0.0;
	FxaaFloat fxaaConsoleEdgeSharpness = 8.0;
	FxaaFloat fxaaConsoleEdgeThreshold = 0.125;
	FxaaFloat fxaaConsoleEdgeThresholdMin = 0.05;
	FxaaFloat4 fxaaConsole360ConstDir = vec4(1.0, -1.0, 0.25, -0.25);

	vec4 fxaa = FxaaPixelShader(
		vUV.xy,
		fxaaConsolePosPos,
		uImage,
		uImage,
		uImage,
		fxaaQualityRcpFrame,
		fxaaConsoleRcpFrameOpt,
		fxaaConsoleRcpFrameOpt2,
		fxaaConsole360RcpFrameOpt2,
		fxaaQualitySubpix,
		fxaaQualityEdgeThreshold,
		fxaaQualityEdgeThresholdMin,
		fxaaConsoleEdgeSharpness,
		fxaaConsoleEdgeThreshold,
		fxaaConsoleEdgeThresholdMin,
		fxaaConsole360ConstDir
	);
    
    oColor = vec4(fxaa.rgb, 1.0);
}