#version 330 core

uniform sampler2D i_color;
uniform sampler2D i_noise;
uniform float uTime;
uniform vec2 uResolution;

in vec2 vTexCoord;
out vec4 o_color;

#define F+texture(i_noise,.3+p.xz*s/3e3)/(s+=s) 
void main()
{
	vec4 p = vec4(vTexCoord / uResolution.xy, 1, 1) - .5, d = p, t;
	p.z += uTime*20.; d.y -= .4;
	for (float i = 1.5; i>0.; i -= .002)
	{
		float s = .9;
		t = F F F F F F;
		o_color = vec4(1, 1., .9, 9) + d.x - t*i;
		if (t.x>p.y*.007 + 1.3)break;
		p += d;
	}
}
/*
void main()
{
	vec2 p = (vTexCoord - vec2(0.5)) * 2.0;
	p.x *= uResolution.x / uResolution.y;
	
	// o_color = vec4(p, 0.0, 1.0);
	// return;

	// camera
	vec3 ro = 4.0*normalize(vec3(sin(3.0), 0.0, cos(3.0)));
	vec3 ta = vec3(0.0, -1.0, 0.0);
	mat3 ca = setCamera(ro, ta, 0.0);
	// ray
	vec3 rd = ca * normalize(vec3(p.xy, 1.5));

	o_color = render(ro, rd, ivec2(vTexCoord - 3.5));
}
*/