#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uMVPTransform;
uniform mat4 uLocalTransform;
uniform mat4 uTransform;

out vec3 vVertexPos;
out vec2 vTexCoord;
out vec3 vNormal; // to remove
out mat3 TBN;

void main() {
	gl_Position = uMVPTransform * aPos;
	vTexCoord = aTexCoord;

	vec3 N = normalize(vec3(uTransform * vec4(aNormal, 0.0)));
	vec3 T = normalize(vec3(uTransform * vec4(aTangent, 0.0)));
	// vec3 B = normalize(vec3(uTransform * vec4(aBitangent, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	TBN = mat3(T, B, N);

	vNormal = normalize(transpose(inverse(mat3(uTransform))) * aNormal); // to remove
	vVertexPos = aPos.xyz;
}