#version 330 core

uniform sampler2D uSpriteMap;
uniform sampler2D uLinearDepth;
uniform vec4 uScreenSize;
uniform float uNear;
uniform float uFar;
uniform float uTime;
uniform float uHasSprite;
uniform vec4 uEmitterAlbedo;
uniform vec4 uEmitterEmissive;
uniform float uSpriteSpeed;
uniform float uSpriteStartFrame;
uniform vec2 uSpriteSubImages;
uniform vec4 uSpriteColor;
uniform float uSpriteDepthFade;

in vec2 vTexCoord;
in float vInstanceID;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

float LinearizeDepth(float depth) {
	return -uFar * uNear / (depth * (uFar - uNear) - uFar);
}

vec2 SubUV(vec2 uv, vec2 subImages, float frame)
{
	vec2 resRcp = vec2(1.0) / subImages;
	float frameInt = frame - fract(frame);
	float uvTileX = mod(frameInt, subImages.x);
	float uvTileY = floor(frameInt * resRcp.x);
	
	vec2 uvTile = (vec2(uvTileX, uvTileY) + uv) * resRcp;
	return uvTile;
}

void main()
{
	vec2 c = vTexCoord * 2.0 - 1.0;
	float mask = 0.5 * (1.0 - clamp(dot(c, c), 0.0, 1.0));

	vec2 uv = vTexCoord.rg;
	float frame = uSpriteStartFrame + uSpriteSubImages.x * uSpriteSubImages.y * fract(-1.0 * uSpriteSpeed * uTime);

	vec2 uvTile0 = SubUV(uv, uSpriteSubImages, frame);
	vec2 uvTile1 = SubUV(uv, uSpriteSubImages, frame + 1);
	vec4 tex1 = texture(uSpriteMap, uvTile1);
	vec4 tex0 = texture(uSpriteMap, uvTile0);
	vec4 tex = mix(tex0, tex1, fract(frame));
	tex *= uSpriteColor;

    color = mix(vec4(mask), tex, uHasSprite) * uEmitterAlbedo + uEmitterEmissive;

	vec2 screenUV = gl_FragCoord.xy * uScreenSize.zw;
	float sceneDepth = texture(uLinearDepth, screenUV).g;
	float pixelDepth = LinearizeDepth(gl_FragCoord.z);
	float depthFade = clamp((sceneDepth - pixelDepth) / uSpriteDepthFade, 0.0, 1.0);	
	color.a *= depthFade;
	
	color.rgb *= color.a;
	
	normal = vec4(0.0);
}