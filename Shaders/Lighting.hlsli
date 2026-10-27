#pragma once


#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2

struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
};

struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float innerCutoff;
    float3 color;
    float intensity;
    float3 ambient;
    float outerCutoff;
    
};

float3 getLightVector(Light light, float3 position, out float attentuation)
{
    float3 l;
    attentuation = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            l = -normalize(light.direction);
            break;
        case LIGHT_POINT:
            l = normalize(light.position - position);
            break;
        case LIGHT_SPOT:
            l = normalize(light.position - position);
            attentuation = smoothstep(light.outerCutoff, light.innerCutoff, dot(-l, light.direction));
            break;
    }
    
    return l;
}