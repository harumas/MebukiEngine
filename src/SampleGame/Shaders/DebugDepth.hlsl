struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VSMain(uint id : SV_VertexID)
{
    VS_OUT output;
    
    float2 texcoord;
    if (id == 0) texcoord = float2(0, 0);
    else if (id == 1) texcoord = float2(1, 0);
    else if (id == 2) texcoord = float2(0, 1);
    else if (id == 3) texcoord = float2(1, 0);
    else if (id == 4) texcoord = float2(1, 1);
    else texcoord = float2(0, 1);
    
    output.uv = texcoord;
    
    float sizeX = 0.5; // width (e.g. half screen)
    float sizeY = 0.5 * (1280.0 / 720.0); // Aspect ratio fix rough estimate
    
    output.pos = float4(texcoord.x * sizeX - 1.0, 1.0 - texcoord.y * sizeY, 0.0, 1.0);
    
    return output;
}

Texture2D<float> depthTexture : register(t0);
SamplerState smp : register(s0);

float4 PSMain(VS_OUT input) : SV_TARGET
{
    float depth = depthTexture.Sample(smp, input.uv);
    float c = pow(depth, 50.0); // Contrast enhancement for visualization
    return float4(c, c, c, 1.0);
}
