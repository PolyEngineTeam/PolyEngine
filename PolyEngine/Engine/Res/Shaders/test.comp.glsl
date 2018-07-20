#version 430

struct Input
{
    int value;
};

struct Output
{
    uint indexLocal;
    uint indexWorkGroup;
    uint indexGlobal;
    uint inputVal;
    uint result;
    vec4 tilePosSS;
};

layout(std430, binding = 0) readonly buffer InputBuffer {
    Input bInputs[];
};

layout(std430, binding = 1) writeonly buffer OutputBuffer {
    Output bOutputs[];
};

layout(local_size_x = 2, local_size_y = 2, local_size_z = 1) in;
void main() {
    uint IndexWorkGroup = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    uint IndexGlobal = gl_GlobalInvocationID.y * (gl_NumWorkGroups.x * gl_WorkGroupSize.x) + gl_GlobalInvocationID.x;

    // bOutputs[gl_WorkGroupID.x] = bInputs[gl_WorkGroupID.x];
    // bOutputs[gl_WorkGroupID.x].value = 0;
    // bOutputs[gl_WorkGroupID.x].value = int(gl_WorkGroupID.x);
    // bOutputs[index].value = int(index);
    // bOutputs[indexLocal].value = int(indexLocal);
    // bOutputs[index].value = int(indexLocal);
    // value = int(indexLocal) + bInputs[index].value;

    // bOutputs[IndexGlobal].indexLocal = gl_LocalInvocationIndex;
    // bOutputs[IndexGlobal].indexWorkGroup = IndexWorkGroup;
    // bOutputs[IndexGlobal].indexGlobal = IndexGlobal;
    // bOutputs[IndexGlobal].inputVal = bInputs[IndexWorkGroup].value;
    // bOutputs[IndexGlobal].result = gl_LocalInvocationIndex + bInputs[IndexWorkGroup].value;

    bOutputs[IndexGlobal].indexLocal = gl_LocalInvocationIndex;
    bOutputs[IndexGlobal].indexWorkGroup = IndexWorkGroup;
    bOutputs[IndexGlobal].indexGlobal = IndexGlobal;
    bOutputs[IndexGlobal].inputVal = bInputs[IndexWorkGroup].value;
    bOutputs[IndexGlobal].result = gl_LocalInvocationIndex + bInputs[IndexWorkGroup].value;
}