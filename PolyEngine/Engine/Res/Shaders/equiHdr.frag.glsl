#version 330 core

in vec3 vPositionInModel;

uniform sampler2D uEquirectangularMap;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(vPositionInModel)); // make sure to normalize localPos
    color.rgb = texture(uEquirectangularMap, uv).rgb;
    normal = vec4(0.0);
}