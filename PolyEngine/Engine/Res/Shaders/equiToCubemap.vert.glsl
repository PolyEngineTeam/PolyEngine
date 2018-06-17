#version 330 core

layout(location = 0) in vec3 aPositionInModel;

out vec3 vPositionInModel;

uniform mat4 uClipFromModel;

void main()
{
    vPositionInModel = aPositionInModel;
    gl_Position = uClipFromModel * vec4(aPositionInModel, 1.0f);
}