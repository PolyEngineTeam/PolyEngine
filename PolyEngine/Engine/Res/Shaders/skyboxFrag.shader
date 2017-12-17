#version 330 core

uniform vec4 uCameraDirection;
uniform sampler3D uCubemap;

in vec2 vTexCoord;
out vec4 o_color;

void main()
{
    o_color.rgb = uCameraDirection.rgb;
    o_color.a = 1.0;
	// color = vec4(texture(uCubemap, uCameraDirection.xyz).rgb, vec3(1.0/2.2)), 1.0);
}