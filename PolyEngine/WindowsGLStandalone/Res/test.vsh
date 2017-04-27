#version 330 core
layout(location = 0) in vec4 vertexPosition_modelspace;

uniform mat4 uMVP;

void main(){
  gl_Position = uMVP * vertexPosition_modelspace;
}