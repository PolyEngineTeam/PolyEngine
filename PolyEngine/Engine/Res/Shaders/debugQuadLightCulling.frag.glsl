#version 430 core

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
};

layout(std430, binding = 0) readonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

uniform float near;
uniform float far;

uniform int workGroupsX;
uniform int workGroupsY;

uniform sampler2D uTexture;

in vec2 vUV;
out vec4 fragColor;

// Need to linearize the depth because we are using the projection
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	// Determine which tile this pixel belongs to
    ivec2 location = ivec2(gl_FragCoord.xy); // (800, 600)
    ivec2 WorkGroupSize = ivec2(16);
    ivec2 NumWorkGroups = ivec2(workGroupsX, workGroupsY);
    ivec2 WorkGroupID = (location / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    
	float minDepth = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].result);
    float maxDepth = uintBitsToFloat(outputBuffer.data[IndexWorkGroup].input);

	vec3 tex = texture(uTexture, vUV).rgb;
    float depth = LinearizeDepth(tex.r)/far;
    tex = pow(tex, vec3(0.4545));
    // fragColor = vec4(vec3(depth), 1.0);
    // fragColor = vec4(vec2(location) / vec2(256.0, 192.0), 0.0, 1.0);
    // fragColor = vec4(vec2(WorkGroupID) / NumWorkGroups, 0.0, 1.0);
    // fragColor = vec4(vec3(minDepth), 1.0);
    fragColor = vec4(depth, minDepth, maxDepth, 1.0);
}