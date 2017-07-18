#version 330 core

uniform sampler2D i_color;

in vec2 vTexCoord;

out vec4 color;

void main(){
  vec4 texColor = texture(i_color, vTexCoord);
  color = texColor + vec4(0.2,0,0,0);
}