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
	// dot(rd, p-ro)
	float depth = texture(uDepth, vUV).g;
    // vec2 screenPos = vUV * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    // vec2 screenPos = vUV * vec2(2.0, -2.0) - vec2(1.0, -1.0);
    vec2 screenPos = vUV * vec2(2.0) - vec2(1.0);
    vec4 invCoords = uWorldFromView * vec4(screenPos, depth, 1.0);
    
	// vec3 worldPos = invCoords.xyz / invCoords.www;

    vec4 coordsPrevFrame = uPrevClipFromWorld * invCoords;
    coordsPrevFrame.xyz /= coordsPrevFrame.www;
	vec4 prevPos = coordsPrevFrame;
    // vec4 prevPos = NDCFromClip(coordsPrevFrame);

    vec2 blurDirection = prevPos.xy * uScale * 0.5;

	vec4 result = texture(uImage, vUV);
    int nSamples = 10;
    float weight = 1.0;
    for (int i = 1; i < nSamples; ++i)
    {
		// get offset in range [-0.5, 0.5]:
        float t = float(i) / float(nSamples - 1);
        // float sampleWeight = smoothstep(0.0, 1.0, 1.0 - abs(t * 2.0 - 1.0));
        vec2 offset = blurDirection * (t - 0.5);
  	
		// sample & add to result:
        result += texture(uImage, vUV + offset);
        weight += 1.0;
    }
 	
    result /= weight;

	// oColor = vec4(vec3(fract(0.2 * depth)), 1.0);
    // oColor = vec4(screenPos, 0.0, 1.0);
    // oColor = vec4(fract(vec3(screenPos, depth)), 1.0);
    // oColor = vec4(fract(0.2 * invCoords.xyz), 1.0);
	// oColor = vec4(fract(0.5 * worldPos.xyz), 1.0);
	// oColor = vec4(coordsPrevFrame.xy * 0.5 + 0.5, 0.0, 1.0);
    // oColor = vec4(blurDir, 0.0, 1.0);
    // oColor = vec4(prevPos.xz, 0.0, 1.0);
    // oColor = vec4(blurDirection, 0.0, 1.0);
	oColor = vec4(result.rgb, 1.0);
}

/*
float4 ResolveMotionVectors(VS_OUTPUT_POSTFX i) : SV_Target
{
    float textureSampleDepth = InputTexture.SampleLevel(pointSampler, i.uv, 0).r;
    float2 screenPos = float2(i.uv * float2(2,-2) + float2(-1,1) );
    float4 invCoords = mul( invViewProjMatrix, float4(screenPos, textureSampleDepth, 1.0f) ); 

    //float3 worldPos = invCoords.xyz / invCoords.www;

    float4 coordsPrevFrame = mul(viewProjMatrixPrevFrame, invCoords);
    coordsPrevFrame /= coordsPrevFrame.wwww;
    float2 uvMotionVector = coordsPrevFrame.xy - screenPos;
    uvMotionVector = uvMotionVector * float2(0.5f,-0.5f);
    
    return float4(uvMotionVector, 0.0f, 0.0f);
}
*/