
struct DirectionalLight
{
    float3 direction;
    float3 color;
    float3 ambientLight;
    matrix lightViewProj;
};
 
cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // CameraFrameData.viewProj (使わないが場所埋めとして必要)
    float3 cameraPos; // CameraFrameData.cameraPos (同上)
    DirectionalLight light;
}

cbuffer cbuff1 : register(b1)
{
    matrix world;
}
 
struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 pos : POSITION)
{
    PSInput result;
    result.position = mul(light.lightViewProj, mul(world, pos));
    return result;
}

void PSMain(PSInput input)
{
}
