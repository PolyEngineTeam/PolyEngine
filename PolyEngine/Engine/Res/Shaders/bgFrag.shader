#version 330 core

uniform sampler2D i_color;
uniform float uTime;

in vec2 vTexCoord;
out vec4 o_color;

void main() {
  // vec4 texColor = texture(i_color, vTexCoord);
  // float distFromCenter = length((vTexCoord - vec2(0.5, 0.5)) * 1.43f);
  // color = texColor * (1.0 - distFromCenter * distFromCenter);
	o_color = vec4(vTexCoord.xy, fract(uTime), 1.0);
}