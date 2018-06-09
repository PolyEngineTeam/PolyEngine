#version 330 core

layout(location = 0) in vec3 aPositionInModel;

out vec3 vUV;

uniform mat4 uClipFromWorld;

void main()
{
    // vec3 textureFlipXFix = vec3(-1.0, 1.0, 1.0);
	// vUV = normalize(aPositionInModel * textureFlipXFix);
    vUV = normalize(aPositionInModel);
    vec4 pos = uClipFromWorld * vec4(aPositionInModel, 1.0f);
	gl_Position = pos.xyww;
}