#version 430 core

struct Light
{
    vec4 Position;
    float Radius;
};

struct VisibleIndex
{
    int index;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

const uint NUM_LIGHTS = 1024;

uniform float time;
uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int lightCount;
uniform int workGroupsX;
uniform int workGroupsY;
uniform mat4 uClipFromWorld;

in vec3 vVertexPos;
in vec2 vTexCoord;

layout(location = 0) out vec4 color;

float distToLight(Light light)
{
    float dist = 1.0;
    vec3 position = light.Position.xyz;
    float radius = light.Radius;
    if (radius > 0.0)
    {
        dist = min(length(vVertexPos - position) / abs(radius), 1.0);
    }
    return fract(radius*dist) * (1.0 - smoothstep(0.99, 1.0, dist));
}

void main()
{
    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16,16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint offset = IndexWorkGroup * NUM_LIGHTS;

    float dist = 0.5;
    uint count = uint(NUM_LIGHTS);
    for (uint i = 0; i < count; i++)
    {
        int lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
        if (lightIndex < 0)
            break;
        dist += 0.1*distToLight(lightBuffer.data[lightIndex]);
    }
    dist = clamp(dist, 0.0, 1.0);

    uint i;
    for (i = 0; i < count && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);
    float ratio = float(i) / (0.1 * float(NUM_LIGHTS));
    ratio = clamp(ratio, 0.0, 1.0);

    color = vec4(fract(vVertexPos), 1.0);
    
    float t = smoothstep(0.5, 0.5, sin(time));
    color.xyz *= mix(ratio, dist, t);
}
