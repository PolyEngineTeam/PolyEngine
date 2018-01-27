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
    float uSpeed = 1.0;
    float uSpeedPow = 1.0;

    float frame = uFrame + uSubImages.x * uSubImages.y * pow(fract(-1.0 * uSpeed * uTime), uSpeedPow);

    vec2 uvTile = SubUV(uv, uSubImages, frame);

    vec4 tex = texture(i_color, uvTile);
    color = vec4(tex);
}