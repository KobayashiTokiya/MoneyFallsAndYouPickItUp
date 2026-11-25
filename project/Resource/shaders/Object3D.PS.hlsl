#include "object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    float32_t4 cameraWorldPosition;
};

struct Camera
{
    float32_t3 worldPosition;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 1. テクスチャから色をサンプリング
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t4 baseColor = gMaterial.color * textureColor;

    if (baseColor.a == 0.0)
    {
        discard;
    }

   // 2. 平行光源による陰影（ライティング）計算
    float32_t3 normal = normalize(input.normal);
    float32_t NdotL = dot(normal, normalize(-gDirectionalLight.direction));
    // Half-Lambert
    float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    // ★ 最低限の明るさ（環境光 0.35f）を足して、地面や影が暗くなりすぎないようにする
    float32_t lightFactor = saturate(cos + 0.35f);
    float32_t3 diffuse = baseColor.rgb * gDirectionalLight.color.rgb * lightFactor * gDirectionalLight.intensity;

    // 3. 環境マップ（映り込み）は1.0倍で足すと白飛びするため、0.15倍で控えめに混ぜる
    float32_t3 cameraToPosition = normalize(input.worldPosition.xyz - gDirectionalLight.cameraWorldPosition.xyz);
    float32_t3 reflectedVector = reflect(cameraToPosition, normal);
    float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);

    output.color.rgb = diffuse + environmentColor.rgb * 0.15f;
    output.color.a = baseColor.a;

    return output;
}