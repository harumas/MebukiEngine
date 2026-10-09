struct DirectionalLight
{
    float3 direction;
    float3 color;
    float3 ambientLight;
    matrix lightViewProj;
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

// シャドウマップ (ライト視点からの深度)
Texture2D shadowMap : register(t1);

// サンプラー
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 worldPos : TEXCOORD1;
    float4 shadowPos : TEXCOORD2;
};

float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal);
float CalcShadowFactor(float4 shadowPos);

PSInput VSMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;
    result.worldPos = mul(world, pos);
    result.position = mul(viewproj, result.worldPos);
    result.normal = normalize(mul(world, normal));
    result.uv = uv;
    result.shadowPos = mul(directionalLight.lightViewProj, result.worldPos);
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // UV座標からサンプリングし、マテリアルのBaseColorで色を乗算する
    float4 color = gTexture.Sample(gSampler, input.uv) * baseColor;
     
    float3 directionNormal = normalize(directionalLight.direction);
    float3 diffuseDirection = CalcLambertDiffuse(directionNormal, directionalLight.color, input.normal);

    // シャドウマップと比較し、影の中なら平行光源の拡散反射を弱める
    float shadowFactor = CalcShadowFactor(input.shadowPos);
    diffuseDirection *= shadowFactor;

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

float CalcShadowFactor(float4 shadowPos)
{
    // NDC(-1〜1)からUV(0〜1)に変換。Yは反転が必要(NDCのY+は上、UVのV+は下)
    float2 shadowUV = shadowPos.xy * 0.5 + 0.5;
    shadowUV.y = 1.0 - shadowUV.y;

    // ライトの正射影の範囲外は判定しない(WRAPサンプラーで無関係な場所を読まないようにする)
    if (shadowUV.x < 0.0 || shadowUV.x > 1.0 || shadowUV.y < 0.0 || shadowUV.y > 1.0)
    {
        return 1.0;
    }

    // XMMatrixOrthographicLHのZは0〜1の範囲なので、そのまま比較できる
    float storedDepth = shadowMap.Sample(gSampler, shadowUV).r;

    if (shadowPos.z > storedDepth)
    {
        // 自分より手前(ライトに近い側)に何かある = 影の中
        return 0.0;
    }

    return 1.0;
}


