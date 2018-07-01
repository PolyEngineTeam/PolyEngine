#version 430 core

in vec2 vUV;

uniform sampler2D uHdrBuffer;

uniform float uExposure;

out vec4 fragColor;

// Uses Reinhard tonemapping https://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
// with an added controllable exposure component
void main()
{
    vec3 color = texture(uHdrBuffer, vUV).rgb;
    vec3 result = vec3(1.0) - exp(-color * uExposure);
    // vec3 result = vec3(1.0) - exp(-color * 2.0);

    fragColor = vec4(result, 1.0);
}
