#version 330 core

uniform sampler2D i_color;
uniform float uTime;

in vec2 vTexCoord;
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
    vec2 uv = vTexCoord.rg;
    vec2 uSubImages = vec2(4.0, 4.0);
    float uSpeedTime = 1.0;
    float uFrame = 0.0;
    float uSpeed = 0.5;
    float uSpeedPow = 1.0;

    float frame = uFrame + uSubImages.x * uSubImages.y * pow(fract(-1.0 * uSpeed * uTime), uSpeedPow);

    vec2 uvTile0 = SubUV(uv, uSubImages, frame);
    vec2 uvTile1 = SubUV(uv, uSubImages, frame + 1);

    vec4 tex0 = texture(i_color, uvTile0);
    vec4 tex1 = texture(i_color, uvTile1);

    vec4 tex = mix(tex0, tex1, fract(frame));

    color = vec4(tex);
}