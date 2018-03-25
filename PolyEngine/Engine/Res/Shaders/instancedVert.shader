#version 330 core

layout(location = 0) in vec3 aVertexInInstance;
layout(location = 1) in vec2 aTexCoord;
layout(location = 3) in mat4 aModelFromInstance;

out vec2 vTexCoord;
out float vInstanceID;

uniform mat4 uScreenFromView;
uniform mat4 uViewFromWorld;
uniform mat4 uWorldFromModel;
uniform float uTime;

void main()
{
	vec4 VertexInInstance = vec4(aVertexInInstance, 1.0);
    vec4 ParticleCenterInWorld = uWorldFromModel * aModelFromInstance * vec4(0.0, 0.0, 0.0, 1.0);

    vec3 CameraRightInWorld = vec3(uViewFromWorld[0][0], uViewFromWorld[1][0], uViewFromWorld[2][0]);
	vec3 CameraUpInWorld = vec3(uViewFromWorld[0][1], uViewFromWorld[1][1], uViewFromWorld[2][1]);
    vec2 BillboardSize = vec2(aModelFromInstance[0][0], aModelFromInstance[1][1]);
    vec4 VertexInWorld = ParticleCenterInWorld
		+ vec4(CameraRightInWorld, 0.0) * aVertexInInstance.x * BillboardSize.x
		+ vec4(CameraUpInWorld, 0.0)	* aVertexInInstance.y * BillboardSize.y;

    vec4 VertexInScreen = uScreenFromView * uViewFromWorld * VertexInWorld;

    gl_Position = VertexInScreen;
    vTexCoord = aTexCoord;
    vInstanceID = gl_InstanceID;
}