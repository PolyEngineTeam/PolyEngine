#version 330 core

uniform sampler2D i_color;
uniform float uVignetteIntensity;
in vec2 vTexCoord;
out vec4 color;

void main(){
  vec4 texColor = texture(i_color, vTexCoord);
  float distFromCenter = length((vTexCoord - vec2(0.5, 0.5)) * 1.43f);
  color = texColor * mix(1.0, (1.0 - distFromCenter * distFromCenter), uVignetteIntensity);
}