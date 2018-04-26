#version 430 core

struct Light
{
    vec4 Position;
    float Radius;
};

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
    vec4 tilePosSS;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int workGroupsX;
uniform int workGroupsY;
uniform mat4 uClipFromWorld;

in vec3 vVertexPos;
in vec2 vTexCoord;

#define NUM_LIGHTS 110

layout(location = 0) out vec4 color;

void main()
{
    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
	float visibleLights = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].result);
    visibleLights = clamp(visibleLights, 0.0, 1.0);
    
	float minDistToLight = 100.0;
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        Light light = lightBuffer.data[i];
        float distToLight = length(light.Position.xyz - vVertexPos.xyz) / light.Radius;
        minDistToLight = min(minDistToLight, distToLight);
    }

    // color = vec4(1.0, 0.0, 0.0, 1.0);
    color = vec4(
				  vec3(0.01)
				+ 0.5*vec3(visibleLights)
				+ 0.5*vec3(smoothstep(0.0, 0.001, 1.0-minDistToLight))
			, 1.0);
}