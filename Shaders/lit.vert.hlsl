Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
    float4x4 mvp;
    float4 color;
};

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : TEXCOORD1;
};

Output main(Input input)
{
    Output output;

    output.texcoord = input.texcoord;
    output.color = float4(0, 1, 0, 1);
    output.position = mul(mvp, float4(input.position, 1.0f));
    
    return output;
}