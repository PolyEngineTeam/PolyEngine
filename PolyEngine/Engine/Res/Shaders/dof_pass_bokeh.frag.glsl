#version 430 core

in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uDepth;

uniform vec4 uRes;
uniform float uDOFPoint;
uniform float uDOFRange;
uniform float uDOFSize;

out vec4 oColor;

float depthToMask(float depth)
{
    float d = clamp(abs(depth - uDOFPoint) / uDOFRange, 0.0f, 1.0f);
    return smoothstep(0.5, 1.0, d);
}

vec3 bokeh(vec2 uv, float rad)
{ // based on dof by Jochen "Virgill" Feldkötter, Alcatraz / Rhodium 4k Intro liquid carbon
  // simplyfied version of Dave Hoskins blur
  // now bokeh is not cut within dof mask, added alpha blending based on difference of dof mask samples
    // const float GA = 2.399;
    // const mat2 rot = mat2(cos(GA), sin(GA), -sin(GA), cos(GA));
    // vec4 acc = vec4(texture(uImage, uv).rgb, rad);
    // float d = rad;
    // vec2 pixel = 0.0001 * vec2(5. * R.y / R.x, 7.);
    // vec2 angle = vec2(0, rad);
    // for (int j = 0; j < 80; j++)
    // {
    //     rad += 1. / rad;
    //     angle *= rot;
    //     vec3 col = texture(uImage, uv + pixel * (rad - 1.) * angle).rgb;
    //     float depth = texture(uDepth, uv + pixel * (rad - 1.) * angle).g;
    //     acc.rgb = max(acc.rgb, col.rgb);
    //     acc.a = max(acc.a, abs(d - depthToMask(depth)));
    // }
    // return acc;

    const float GA = 2.399;
    const mat2 rot = mat2(cos(GA), sin(GA), -sin(GA), cos(GA));

    vec3 acc = vec3(0);
    vec2 pixel = uDOFSize * vec2(.002 * uRes.y / uRes.x, .002), angle = vec2(0, rad);;
    // rad = 1.;
    for (int j = 0; j < 80; j++)
    {
        rad += 1. / (rad + 0.0001);
        angle *= rot;
        vec2 offset = pixel * (rad - 1.) * angle;
        vec4 col = texture(uImage, uv + offset);
        acc += col.xyz;
    }
    return acc / 80.;
}

void main()
{
    float depth = texture(uDepth, vUV).g;
    float dofMask = depthToMask(depth);
    oColor = vec4(bokeh(vUV, dofMask), dofMask);
}