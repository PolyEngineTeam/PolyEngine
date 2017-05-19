#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 2) in vec3 a_normal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 u_projection;
uniform mat4 u_viewModel;
uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(a_position, 1.0f); 
    mat3 normalMatrix = mat3(transpose(inverse(u_viewModel)));
    vs_out.normal = normalize(vec3(u_projection * vec4(normalMatrix * a_normal, 1.0)));
}