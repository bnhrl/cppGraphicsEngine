#pragma once


#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2

static const uint MAX_LIGHTS = 8;

struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
    float3 padding1;
};

struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float range;
    float3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
    float2 padding1;
};

float calculateAttentuation(float distance, float range)
{
    return saturate(1 - (distance / range));
}

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
        {
            float3 toLight = light.position - position;
            float distance = length(toLight);
            attentuation = calculateAttentuation(distance, light.range);
            l = toLight / distance;
        }
            break;
        case LIGHT_SPOT:
        {
            float3 toLight = light.position - position;
            float distance = length(toLight);
            l = toLight / distance;
            attentuation = calculateAttentuation(distance, light.range);
            attentuation *= smoothstep(light.innerCutoff, light.outerCutoff, dot(-l, light.direction));
        }
            break;
    }
    
    return l;
}

float3 calculateDiffuse(float3 radiance, float3 n, float3 l)
{
    // diffuse is the dot product of the normal and light vector 
    return radiance * saturate(dot(n, l));
}

float3 calculateSpecular(float3 radiance, float3 n, float3 l, float3 v, Material material)
{
    float3 specular = float3(0, 0, 0);
    if (dot(n, l) > 0)
    {
        float3 r = reflect(-l, n);
        float3 h = normalize(v + l);
        float3 hDotN = saturate(dot(h, n));
        hDotN = pow(hDotN, material.shininess);
        specular = radiance * hDotN;
    }
    return specular;
}

void calculateLighting(Light light, Material material, float3 position, float3 n, float3 v,
                       out float3 diffuse, out float3 specular)
{
    float attenuation;
    float3 l = getLightVector(light, position, attenuation);

    // light arriving at the surface
    float3 radiance = light.color * light.intensity * attenuation;

    diffuse += calculateDiffuse(radiance, n, l);
    specular += calculateSpecular(radiance, n, l, v, material);
}