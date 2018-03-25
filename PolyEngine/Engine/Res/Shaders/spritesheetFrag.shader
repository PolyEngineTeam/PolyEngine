#version 330 core

uniform sampler2D i_color;
uniform float uTime;
uniform vec2 uSubImages;
uniform vec4 uColor;
uniform float uStartFrame;
uniform float uSpeed;

in vec2 vUV;
out vec4 color;

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
    float frame = uStartFrame + uSubImages.x * uSubImages.y * fract(-1.0 * uSpeed * uTime);

    vec2 uvTile0 = SubUV(vUV, vec2(uSubImages), frame);
    vec2 uvTile1 = SubUV(vUV, vec2(uSubImages), frame + 1);

    vec4 tex0 = texture(i_color, uvTile0);
    vec4 tex1 = texture(i_color, uvTile1);

    vec4 tex = mix(tex0, tex1, fract(frame));

    color = uColor * tex;
}