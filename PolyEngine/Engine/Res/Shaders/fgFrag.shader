#version 330 core

uniform sampler2D i_color;
uniform float uTime;
uniform vec2 uResolution;

in vec2 vTexCoord;
out vec4 color;

void main(){
  vec4 texColor = texture(i_color, vTexCoord);
  vec2 p = (vTexCoord - vec2(0.5)) * 2.0;
  p.x *= uResolution.x / uResolution.y;
  p.x += 0.1 * sin(uTime);
  color = texColor *smoothstep(0.1, 0.11, length(p));
}