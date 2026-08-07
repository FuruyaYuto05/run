#include "object3d.hlsli"


struct Material
{
    float32_t4 color;
    int enableLighting;
    float32_t3 padding;
    float32_t4x4 uvTransform;
    float shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);
struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    if (gMaterial.enableLighting == 0)
    {
        output.color = gMaterial.color * textureColor;
        return output;
    }

    float32_t3 normal = normalize(input.normal);
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float32_t3 lightDirection = normalize(gDirectionalLight.direction);
    float cos = saturate(dot(normal, -lightDirection));

    // Blinn-Phong: 光の逆方向と視線方向の中間ベクトルを使って鏡面反射を求める
    float32_t3 halfVector = normalize(-lightDirection + toEye);
    float NdotH = dot(normal, halfVector);
    float specularPower = pow(saturate(NdotH), gMaterial.shininess);
    float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPower;
    output.color.rgb = diffuse + specular;
    output.color.a = gMaterial.color.a * textureColor.a;
 
    return output;
}
