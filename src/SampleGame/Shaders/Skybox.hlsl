struct DirectionalLight
{
    float3 direction;
    float3 color;
    float3 ambientLight;
};

struct PointLight
{
    float3 position;
    float3 color;
    float range;
};

cbuffer cbuff0 : register(b0)
{
    matrix viewproj;
    float3 cameraPos;
    DirectionalLight directionalLight;
    PointLight pointLight;
}

cbuffer cbuff1 : register(b1)
{
    matrix world;
}

// アルベド用テクスチャ
Texture2D gTexture : register(t0);

// サンプラー
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
};

PSInput VSMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;
    result.worldPos = mul(world, pos);
    result.position = mul(viewproj, result.worldPos);
    result.normal = normalize(mul(world, normal));
    result.uv = uv;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // UV座標からサンプリング
    float4 color = gTexture.Sample(gSampler, input.uv);
     
    return color;
}

