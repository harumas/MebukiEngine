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

cbuffer cbuff2 : register(b2)
{
    float4 baseColor;
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

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal);

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
     
    float3 directionNormal = normalize(directionalLight.direction);
    float3 diffuseDirection = CalcLambertDiffuse(directionNormal, directionalLight.color, input.normal);

    float3 lightDir = normalize(input.worldPos - pointLight.position);
    float3 distance = length(input.worldPos - pointLight.position);

    float3 diffusePoint = pointLight.color * max(0.0f, dot(input.normal, -lightDir));

	// 影響率は距離に比例して小さくなっていく
    //float affect = (1.0f - distance / pointLight.range) / (1.0f + distance * a * a);
    float affect = 1.0f / max(0.01f, distance * distance);

    // step-11 拡散反射光と鏡面反射光に減衰率を乗算して影響を弱める
    diffusePoint *= affect;

    // step-12 2つの反射光を合算して最終的な反射光を求める
    float3 diffuse = diffusePoint + diffuseDirection;

    // 拡散反射光と鏡面反射光を足し算して、最終的な光を求める
    float3 light = diffuse + directionalLight.ambientLight;
    float4 finalColor = float4(color * light, 1);

    return finalColor;
}

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, -lightDirection);

    t = t * 0.5f + 0.5;
    t = t * t;

    // 拡散反射光を計算する
    return lightColor * t;
}


