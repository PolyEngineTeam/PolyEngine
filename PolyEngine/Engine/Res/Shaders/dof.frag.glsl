#version 430 core

in vec2 vUV;

uniform sampler2D uImage;
uniform sampler2D uDepth;

uniform vec4 uRes;
uniform float uDOFpoint;
uniform float uDOFrange;
uniform float uDOFsize;
uniform float uTime;

out vec4 oColor;

// #define R vec2(1280.0, 760.0)
#define T uTime
#define R uRes
#define F gl_FragCoord

float nrand(vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 filmGrainColor(vec2 uv, float offset)
{ // by ma (lstGWn)
    vec4 uvs;
    uvs.xy = uv + vec2(offset, offset);
    uvs.zw = uvs.xy + 0.5 * vec2(1.0 / R.x, 1.0 / R.y);

    uvs = fract(uvs * vec2(21.5932, 21.77156).xyxy);

    vec2 shift = vec2(21.5351, 14.3137);
    vec2 temp0 = uvs.xy + dot(uvs.yx, uvs.xy + shift);
    vec2 temp1 = uvs.xw + dot(uvs.wx, uvs.xw + shift);
    vec2 temp2 = uvs.zy + dot(uvs.yz, uvs.zy + shift);
    vec2 temp3 = uvs.zw + dot(uvs.wz, uvs.zw + shift);

    vec3 r = vec3(0.0, 0.0, 0.0);
    r += fract(temp0.x * temp0.y * vec3(95.4337, 96.4337, 97.4337));
    r += fract(temp1.x * temp1.y * vec3(95.4337, 96.4337, 97.4337));
    r += fract(temp2.x * temp2.y * vec3(95.4337, 96.4337, 97.4337));
    r += fract(temp3.x * temp3.y * vec3(95.4337, 96.4337, 97.4337));

    return r * 0.25;
}

vec2 barrelDistortion(vec2 coord, float amt, float zoom)
{ // based on gtoledo3 (XslGz8)
  // added zoomimg
    vec2 cc = coord - 0.5;
    vec2 p = cc * zoom;
    coord = p + 0.5;
    float dist = dot(cc, cc);
    return coord + cc * dist * amt;
}

vec3 sharpen()
{ // by NickWest (lslGRr)
    vec2 uv = F.xy / R.xy;
    vec2 step = R.zw;
    float scale = 1.5;
    vec3 texA = texture(uImage, uv + vec2(-step.x, -step.y) * scale).rgb;
    vec3 texB = texture(uImage, uv + vec2(step.x, -step.y) * scale).rgb;
    vec3 texC = texture(uImage, uv + vec2(-step.x, step.y) * scale).rgb;
    vec3 texD = texture(uImage, uv + vec2(step.x, step.y) * scale).rgb;
    vec3 around = 0.25 * (texA + texB + texC + texD);
    vec3 center = texture(uImage, uv).rgb;
    vec3 col = center + (center - around) * 1.0;
    return col;
}

float depthToMask(float depth)
{
    // float DOFpoint = 1000.0;
    // float DOFramp = 800.0;
    float d = clamp(abs(depth - uDOFpoint) / uDOFrange, 0.0f, 1.0f);
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

    // float uDOFsize = 0.2;

    const float GA = 2.399;
    const mat2 rot = mat2(cos(GA), sin(GA), -sin(GA), cos(GA));

    vec3 acc = vec3(0);
    vec2 pixel = uDOFsize * vec2(.002 * uRes.y / uRes.x, .002), angle = vec2(0, rad);;
    rad = 1.;
    for (int j = 0; j < 80; j++)
    {
        rad += 1. / rad;
        angle *= rot;
        vec4 col = texture(uImage, uv + pixel * (rad - 1.) * angle);
        acc += col.xyz;
    }
    return acc / 80.;
}

void main()
{
    // float Far = 2000.0;

    vec2 uv = barrelDistortion(vUV, 0.1, 0.96);

    vec3 image = texture(uImage, uv).rgb;

    float depth = texture(uDepth, uv).g;
    // float DOFmask = clamp(abs(depth - DOFpoint) / DOFramp, 0.0f, 1.0f);
 

    // vec3 color = vec3(DOFmask, image.rg);
    // vec3 result = pow(color, vec3(1.0 / uGamma));

    // vec3 sharp = sharpen();
    float dofMask = depthToMask(depth);
    vec3 sharp = image;
    vec3 dof = bokeh(uv, dofMask);

    vec3 c = dof;

    c *= 1.0 - 0.25 * filmGrainColor(0.5 * uv, T).rgb;
    
    vec2 v = 2. * (uv - .5);
    //v.y *= 2.39 * R.y / R.x;
    v = clamp((v * .5) + .5, 0., 1.);
    // c.rgb = vec3(1.0); // uncomment to see only vignette
    c *= 0.25 + 0.75 * pow(16.0 * v.x * v.y * (1.0 - v.x) * (1.0 - v.y), 0.25);
    // fragColor = vec4(mix(sharp, dof.rgb, dof.a), 1.0);

    oColor = vec4(c.rgb, 1.0);
    // oColor.r = dofMask;
}