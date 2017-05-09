#version 330 core

uniform sampler2D ourTexture;

in vec2 vTexCoord;

out vec4 color;

void main(){
  vec4 texColor = texture(ourTexture, vTexCoord);
  if(texColor.a < 0.1)
        discard;
  color = texColor;
}