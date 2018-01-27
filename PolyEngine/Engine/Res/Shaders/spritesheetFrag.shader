#version 330 core

uniform sampler2D i_color;
uniform float uTime;

in vec2 vTexCoord;
out vec4 color;

void main()
{
    vec2 uv = vTexCoord.rg;
    vec2 uSubImages = vec2(4.0, 4.0);
    float uFrame = 16.0 * fract(-1.0 * uTime);

    vec2 resRcp = vec2(1.0) / uSubImages;
    float frame = uFrame - fract(uFrame);
    float uvTileX = mod(frame, uSubImages.x);
    float uvTileY = floor(frame * resRcp.x);
    vec2 uvTile = (vec2(uvTileX, uvTileY) + uv) * resRcp;
    vec4 tex = texture(i_color, uvTile);
    color = vec4(tex);
}