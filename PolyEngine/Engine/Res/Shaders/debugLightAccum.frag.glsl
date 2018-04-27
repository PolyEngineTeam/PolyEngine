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

#define NUM_LIGHTS 4

layout(location = 0) out vec4 color;

void main()
{
    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    
	float visibleLights = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].result);
    float visibleLightsNormalized = clamp(visibleLights / 10.0, 0.0, 1.0);
    visibleLights = clamp(visibleLights, 0.0, 1.0);

	float minDistToLight = 1000000.0;
    for (uint i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 position = lightBuffer.data[i].Position.xyz;
        float radius = lightBuffer.data[i].Radius;
        float distToLight = min(length(vVertexPos - position) / radius, 1.0);
        minDistToLight = min(minDistToLight, distToLight);
    }

    // color = vec4(1.0, 0.0, 0.0, 1.0);
    // vec3 tileColor = visibleLights * mix(vec3(0.5, 0.0, 0.0), vec3(0.5, 0.0, 0.0), visibleLightsNormalized);
    color = vec4(
				  vec3(0.1)
				+ vec3(0.9) * minDistToLight
			//		* mix(0.5, 1.0, visibleLightsNormalized)
			//		* mix(0.5, 1.0, minDistToLight)
			, 1.0);
}