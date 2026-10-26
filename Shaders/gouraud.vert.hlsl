#include "Lighting.hlsli"

cbuffer Uniforms : register(b0, space1)
{
    float4x4 mvp;
    float4x4 model;
};

cbuffer LightUniforms : register(b1, space1)
{
    Light light;
};

cbuffer MaterialUniforms : register(b2, space1)
{
    Material material;
};

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 diffuse : TEXCOORD1;
    float3 specular : TEXCOORD2;
};

Output main(Input input)
{
    Output output;
    output.texcoord = (input.texcoord * material.tiling) + material.offset;
    output.position = mul(mvp, float4(input.position, 1.0f));

    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;
    
    // DIFFUSE
    float3 n = normalize(mul((float3x3)model, input.normal));
    
    float3 l;
    if (light.type == LIGHT_DIRECTIONAL)
    {
        l = -normalize(light.direction);
    }
    else if (light.type == LIGHT_POINT)
    {
        l = normalize(light.position - worldPosition);
    }
    
    float nDotL = saturate(dot(n, l));
    
    float3 diffuse = light.color * light.intensity * nDotL;
    output.diffuse = diffuse + light.ambient;
    
    if (nDotL)
    {
        float shininess = 16.0f;
        float3 r = reflect(-l, n) * shininess;
        
    }
    
    return output;
}