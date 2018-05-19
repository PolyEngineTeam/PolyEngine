#version 430 core

in VERTEX_OUT{
	vec3 positionInWorld;
	vec2 UV;
	vec3 normal;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} fragment_in;

struct Light
{
    vec4 Position;
    vec4 Color;
    vec4 RangeIntensity;
};

struct VisibleIndex
{
    int Index;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	Light data[];
} bLightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} bVisibleLightIndicesBuffer;

const uint MAX_NUM_LIGHTS = 1024;

uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uNormalMap;
uniform int uLightCount;
uniform int uWorkGroupsX;
uniform int uWorkGroupsY;

layout(location = 0) out vec4 oColor;

vec3 GetIrradiance(Light light, vec3 toCamera, vec3 normal, vec3 diffuse, float specular)
{
	// light faloff based on Real Shading in Unreal Engine 4 http://gamedevs.org/uploads/real-shading-in-unreal-engine-4.pdf
    
	vec3 position = light.Position.xyz;
	float range = light.RangeIntensity.x;
    float dist = length(fragment_in.positionInWorld - position);
	float falloff = pow(clamp(1.0 - pow(dist / range, 4.0), 0.0, 1.0), 2.0) / (pow(dist, 2.0) + 1.0);
     
    // vec3 lightPositionInTangent = fragment_in.TBN * position;
	// 
    // vec3 L = normalize(lightPositionInTangent - fragment_in.tangentFragmentPosition);
    // vec3 V = normalize(toCamera);
    // vec3 N = normalize(normal);
	// 
    // 
	// // diffuse
    // float NdotL = max(dot(N, L), 0.0);
	// 
	// // specular
    // vec3 H = normalize(L + V);
    // float NdotH = pow(max(dot(N, H), 0.0), 16.0);
    // if (NdotL <= 0.0)
    // {
    //     NdotH = 0.0;
    // }
	// 
    // return light.Color.rgb * light.RangeIntensity.y * (diffuse * NdotL + diffuse * NdotH) * falloff;

    // vec4 lightColor = light.color;
    vec3 tangentLightPosition = fragment_in.TBN * light.Position.xyz;
    // float lightRadius = light.paddingAndRadius.w;
	
	// Calculate the light attenuation on the pre-normalized lightDirection
    vec3 lightDirection = tangentLightPosition - fragment_in.tangentFragmentPosition;
    // float attenuation = attenuate(lightDirection, lightRadius);
	
	// Normalize the light direction and calculate the halfway vector
    lightDirection = normalize(lightDirection);
    vec3 halfway = normalize(lightDirection + toCamera);
	
	// Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
    float NdotL = max(dot(lightDirection, normal), 0.0);
	// How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
    float NdotH = pow(max(dot(normal, halfway), 0.0), 32.0);
	
	// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
    if (NdotL == 0.0)
    {
        NdotH = 0.0;
    }
	
    return light.Color.rgb * light.RangeIntensity.y * ((diffuse.rgb * NdotL) + (diffuse.rgb * specular * NdotH)) * falloff;
}

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = texture(uDiffuseTexture, fragment_in.UV);
    vec3 normal = texture(uNormalMap, fragment_in.UV).rgb;
    float specular = texture(uSpecularTexture, fragment_in.UV).r;
	// oColor.rgb = diffuse.rgb; return;
    // oColor.rgb = normal; return;
    // oColor.rgb = vec3(specular); return;

    normal = normalize(normal * 2.0 - 1.0);

    if (diffuse.a < 0.5)
    {
        discard;
    }

    // if (isnan(normal.x) || dot(normal, normal) < 0.9)
    // {
    //     normal = fragment_in.normal;
    // }

    // vec3 viewDirection = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);
    vec3 toCamera = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);
 
    color.rgb += vec3(0.0, 0.0, 0.001); // ambient

    ivec2 WorkGroupSize = ivec2(16, 16);
    ivec2 NumWorkGroups = ivec2(uWorkGroupsX, uWorkGroupsY);
    ivec2 WorkGroupID = (ivec2(gl_FragCoord.xy) / WorkGroupSize);
    uint IndexWorkGroup = WorkGroupID.y * NumWorkGroups.x + WorkGroupID.x;
    uint TileOffset = IndexWorkGroup * MAX_NUM_LIGHTS;

    uint Count = uint(uLightCount);
    for (uint i = 0; i < Count; ++i)
	{
        int lightIndex = bVisibleLightIndicesBuffer.data[TileOffset + i].Index;
        Light light = bLightBuffer.data[lightIndex];
        color.rgb += GetIrradiance(light, toCamera, normal, diffuse.rgb, specular);
        
		if (bVisibleLightIndicesBuffer.data[TileOffset + i + 1].Index == -1)
            break;
    }		

    oColor = color;
}