#include "Lighting.hlsli"

cbuffer VertexUniforms : register(b0, space1)
{
    float4x4 mvp;
    float4x4 model;
    float3 cameraPosition;
    float padding1;
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
    float3 radiance = light.color * light.intensity;
    
    float attentuation;
    float3 l = getLightVector(light, worldPosition, attentuation);
    float3 n = normalize(mul((float3x3)model, input.normal));
    
    
    // DIFFUSE
    float nDotL = saturate(dot(n, l));
    float3 diffuse = radiance * nDotL * attentuation;
    output.diffuse = diffuse + light.ambient;
    
    
    // SPECULAR
    output.specular = float3(0.0f, 0.0f, 0.0f);
    if (nDotL > 0)
    {
        float3 r = reflect(-l, n);
        float3 v = normalize(cameraPosition - worldPosition);
        float3 h = normalize(v + l);
        float3 hDotN = saturate(dot(h, n));
        hDotN = pow(hDotN, material.shininess);
        output.specular = radiance * hDotN * attentuation;
    }
    
    return output;
}