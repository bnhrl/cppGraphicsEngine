#include "Lighting.hlsli"
#include "Uniforms.hlsli"

Texture2D<float4> base_texture : register(t0, FRAGMENT_TEXTURE_SPACE);
SamplerState base_sampler : register(s0, FRAGMENT_TEXTURE_SPACE);

Texture2D<float4> specular_texture : register(t1, FRAGMENT_TEXTURE_SPACE);
SamplerState specular_sampler : register(s1, FRAGMENT_TEXTURE_SPACE);

Texture2D<float4> emissive_texture : register(t2, FRAGMENT_TEXTURE_SPACE);
SamplerState emissive_sampler : register(s2, FRAGMENT_TEXTURE_SPACE);

Texture2D<float4> normal_texture : register(t3, FRAGMENT_TEXTURE_SPACE);
SamplerState normal_sampler : register(s3, FRAGMENT_TEXTURE_SPACE);

cbuffer ObjectUniforms : register(OBJECT_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Object object;
};

cbuffer FrameUniforms : register(FRAME_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Frame frame;
}

cbuffer LightUniforms : register(LIGHT_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Lights lights;
};

cbuffer MaterialUniforms : register(MATERIAL_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Material material;
};

struct Input
{
    float2 texcoord : TEXTCOORD0;
    float3 worldPosition : TEXCOORD1;
    float3 normal : TEXCOORD2;
    float3 tangent : TEXCOORD3;
};

struct Output
{
    float4 color : SV_Target0;
};

Output main(Input input)
{
    Output output;
    
    float specular_mask = specular_texture.Sample(specular_sampler, input.texcoord).r;
    float3 emissive = emissive_texture.Sample(emissive_sampler, input.texcoord).rgb;
    
    float4 albedo = base_texture.Sample(base_sampler, input.texcoord) * material.color;
    float3 specular = 0.0;
    float3 normal = normal_texture.Sample(normal_sampler, input.texcoord).rgb * 2.0 - 1.0;
    normal *= material.normalStrength;
    float3 diffuse = lights.ambient;
    
    float3 nNormal = normalize(input.normal);
    float3 t = normalize(input.tangent - nNormal * dot(input.tangent, nNormal));
    float3 b = cross(nNormal, t);
    float3x3 tbn = float3x3(t, b, nNormal);
    float3 n = normalize(mul(normal, tbn));
    
    float3 v = normalize(frame.cameraPosition - input.worldPosition);
    for (uint i = 0; i < lights.numLights; i++)
    {
        
        calculateLighting(lights.lights[i], material, input.worldPosition, n, v, diffuse, specular);
    }
    
    output.color = float4(albedo.rgb * diffuse + specular * material.specularColor * specular_mask + emissive, albedo.a);
    
    return output;
}
