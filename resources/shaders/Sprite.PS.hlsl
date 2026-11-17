struct Material
{
    float4 color;
    int enableLighting;
    int isGrayscale;
    float2 padding;
    float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float4 sampledColor = gTexture.Sample(gSampler, input.texcoord);
    if (gMaterial.isGrayscale != 0) {
        const float luminance = dot(sampledColor.rgb, float3(0.299f, 0.587f, 0.114f));
        sampledColor.rgb = luminance.xxx;
    }
    output.color = gMaterial.color * sampledColor;
    return output;
}
