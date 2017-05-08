#version 330 core

uniform sampler2D ourTexture;

in vec2 vTexCoord;

out vec4 color;

void main(){
  color = texture(ourTexture, vTexCoord);
}