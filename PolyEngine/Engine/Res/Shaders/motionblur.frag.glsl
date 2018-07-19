#version 330 core

in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uDepth;
uniform mat4 uWorldFromView;
uniform mat4 uPrevClipFromWorld;
uniform float uScale;

out vec4 oColor;

void main()
{
	float depth = texture(uDepth, vUV).g;
    vec2 screenPos = vUV * vec2(2.0) - vec2(1.0);
    vec4 invCoords = uWorldFromView * vec4(screenPos, depth, 1.0);
    
    vec4 coordsPrevFrame = uPrevClipFromWorld * invCoords;
    coordsPrevFrame.xyz /= coordsPrevFrame.www;

    vec2 blurDirection = coordsPrevFrame.xy * uScale * 0.5;

	vec4 result = texture(uImage, vUV);
    int nSamples = 20;
    float weight = 1.0;
    for (int i = 1; i < nSamples; ++i)
    {
		// get offset in range [-0.5, 0.5]:
        float t = float(i) / float(nSamples - 1);
        float sampleWeight = smoothstep(0.0, 1.0, 1.0 - abs(t * 2.0 - 1.0));
        vec2 offset = blurDirection * (t - 0.5);
  	
		// sample & add to result:
        result += sampleWeight * texture(uImage, vUV + offset);
        weight += sampleWeight;
    }
 	
    result /= weight;

	oColor = vec4(result.rgb, 1.0);
}