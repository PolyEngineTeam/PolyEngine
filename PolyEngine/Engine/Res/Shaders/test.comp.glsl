#version 430

struct Input {
    int value;
};

struct Output {
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint input;
    uint result;
    vec4 tilePosSS;
};

layout(std430, binding = 0) readonly buffer InputBuffer {
	Input data[];
} inputBuffer;

layout(std430, binding = 1) writeonly buffer OutputBuffer {
	Output data[];
} outputBuffer;

layout(local_size_x = 2, local_size_y = 2, local_size_z = 1) in;
void main() {
    uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    uint IndexGlobal = gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x) + gl_GlobalInvocationID.x;

    // outputBuffer.data[gl_WorkGroupID.x] = inputBuffer.data[gl_WorkGroupID.x];
    // outputBuffer.data[gl_WorkGroupID.x].value = 0;
    // outputBuffer.data[gl_WorkGroupID.x].value = int(gl_WorkGroupID.x);
    // outputBuffer.data[index].value = int(index);
    // outputBuffer.data[indexLocal].value = int(indexLocal);
    // outputBuffer.data[index].value = int(indexLocal);
	// value = int(indexLocal) + inputBuffer.data[index].value;

    // outputBuffer.data[IndexGlobal].indexLocal = gl_LocalInvocationIndex;
    // outputBuffer.data[IndexGlobal].indexWorkGroup = IndexWorkGroup;
    // outputBuffer.data[IndexGlobal].indexGlobal = IndexGlobal;
    // outputBuffer.data[IndexGlobal].input = inputBuffer.data[IndexWorkGroup].value;
    // outputBuffer.data[IndexGlobal].result = gl_LocalInvocationIndex + inputBuffer.data[IndexWorkGroup].value;

    outputBuffer.data[IndexGlobal].indexLocal = gl_LocalInvocationIndex;
    outputBuffer.data[IndexGlobal].indexWorkGroup = IndexWorkGroup;
    outputBuffer.data[IndexGlobal].indexGlobal = IndexGlobal;
    outputBuffer.data[IndexGlobal].input = inputBuffer.data[IndexWorkGroup].value;
    outputBuffer.data[IndexGlobal].result = gl_LocalInvocationIndex + inputBuffer.data[IndexWorkGroup].value;
}