//Texture2D<float4> u_texture : register(t0, space2);
//SamplerState u_sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
    float4x4 mvp;
    float4x4 model;
};

cbuffer LightUniforms : register(b1, space1)
{
    float3 lightDirection;
    float padding1;
    float3 lightColor;
    float lightIntensity;
    float3 ambient;
    float padding2;
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
};

Output main(Input input)
{
    Output output;
    output.texcoord = input.texcoord;
    output.position = mul(mvp, float4(input.position, 1.0f));

    // DIFFUSE
    float3 n = normalize(mul((float3x3)model, input.normal));
    float3 l = -lightDirection;
    float nDotL = saturate(dot(n, l));
    
    float3 diffuse = lightColor * lightIntensity * nDotL;
    
    output.diffuse = diffuse + ambient;
    
    return output;
}