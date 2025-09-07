struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

PSInput VSMain(float4 pos : POSITION, float3 normal : NORMAL)
{
    PSInput result;
    result.position = mul(mul(viewproj, world), pos);
    result.normal = mul(world, normal);
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // 紫色 (R=1, G=0, B=1, A=1)
    return float4(1, 0, 1, 1);
}
