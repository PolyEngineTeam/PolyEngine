#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 viewModel;

void main()
{
	gl_Position = projection * viewModel * vec4(position, 1.0f); 
    mat3 normalMatrix = mat3(transpose(inverse(viewModel)));
    vs_out.normal = normalize(vec3(projection * vec4(normalMatrix * normal, 1.0)));
}