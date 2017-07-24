#version 330 core

uniform sampler2D ourTexture;

in vec2 vTexCoord;

layout(location = 0) out vec4 color;

void main(){
  vec4 texColor = texture(ourTexture, vTexCoord);
  if(texColor.a < 0.1)
        discard;
  color = texColor;
}