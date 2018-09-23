#version 330 core

in vec2 vUV;

uniform sampler2D uImage;
uniform float uIsHorizontal;
uniform float uBlurScale;
uniform float uTime;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 oColor;

vec2 hash22(vec2 p) {
	p = fract(p * vec2(5.3983, 5.4427));
    p += dot(p.yx, p.xy +  vec2(21.5351, 14.3137));
	return fract(vec2(p.x * p.y * 95.4337, p.x * p.y * 97.597));
}

void main()
{	
    float rnd = hash22(vUV + vec2(0.01 * uTime)).x; 
    vec2 texelSize = 1.0 / textureSize(uImage, 0); // gets size of single texel
    vec3 result = texture(uImage, vUV).rgb * weight[0]; // current fragment's contribution
    vec2 blurDir = mix(vec2(1.0, 0.0), vec2(0.0, 1.0), step(0.5, uIsHorizontal));	
    blurDir *= mix(uBlurScale, 1.0 + ((uBlurScale - 1.0) * rnd), step(1.0, uBlurScale));
	float offset = mix(texelSize.x, texelSize.y, step(0.5, uIsHorizontal));
    for(int i = 1; i < 5; ++i)
    {
        result += texture(uImage, vUV + blurDir * offset * i).rgb * weight[i];
        result += texture(uImage, vUV - blurDir * offset * i).rgb * weight[i];
    }
    oColor = vec4(result, 1.0);
}