#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>

#include "Math.h"

class DirectXCommon;
class SrvManager;

class ParticleManager
{
public:
    // ==============================
    // シングルトン取得
    // ==============================
    static ParticleManager* GetInstance();

    // ==============================
    // 初期化
    // ==============================
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);


    // ==============================
    // パーティクルグループ作成
    // ==============================
    void CreateParticleGroup(const std::string& name, const std::string& textureFilePath);

    // ==============================
    // 更新
    // ==============================
    void Update(const Math::Matrix4x4& viewProjectionMatrix);

    // ==============================
    // 描画
    // ==============================
    void Draw();

    // ==============================
    // 終了処理
    // ==============================
    void Finalize();

    // ==============================
    // パーティクル発生
    // ==============================
    void Emit(
        const std::string& name,
        const Math::Vector3& position,
        uint32_t count
    );

private:
    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

private:
    // ==============================
    // シングルトン
    // ==============================
    // ==============================
    // DirectXCommon
    // ==============================
    DirectXCommon* dxCommon_ = nullptr;

    // ==============================
    // SRVマネージャ
    // ==============================
    SrvManager* srvManager_ = nullptr;

    // ==============================
    // パーティクル1個分のデータ構造体
    // ==============================
    struct Particle
    {
        Math::Transform transform;
        Math::Vector3 velocity;
        Math::Vector4 color;
        float lifeTime;
        float currentTime;
    };

    // ==============================
    // GPUに送るパーティクルデータ
    // ==============================
    struct ParticleForGPU
    {
        Math::Matrix4x4 WVP;
        Math::Matrix4x4 World;
        Math::Vector4 color;
    };

    // ==============================
    // テクスチャごとのパーティクルグループ
    // ==============================
    struct ParticleGroup
    {
        // ==============================
        // マテリアルデータ
        // テクスチャファイルパスとテクスチャ番号
        // ==============================
        std::string textureFilePath;
        uint32_t textureIndex = 0;

        // ==============================
        // パーティクルのリスト
        // ==============================
        std::list<Particle> particles;

        // ==============================
        // インスタンシング用SRV番号
        // ==============================
        uint32_t instancingSrvIndex = 0;

        // ==============================
        // インスタンシング用リソース
        // ==============================
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

        // ==============================
        // インスタンシング用データ書き込み先
        // ==============================
        ParticleForGPU* instancingData = nullptr;

        // ==============================
        // 描画するインスタンス数
        // ==============================
        uint32_t numInstance = 0;


    };

    // ==============================
    // パーティクルグループ
    // key: グループ名
    // value: パーティクルグループ
    // ==============================
    std::unordered_map<std::string, ParticleGroup> particleGroups_;

    // ==============================
    // 1グループあたりの最大パーティクル数
    // ==============================
    static const uint32_t kNumMaxInstance = 100;

    // ==============================
    // パーティクル用RootSignature
    // ==============================
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // ==============================
    // パーティクル用PipelineState
    // ==============================
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

    // ==============================
    // 頂点バッファ
    // ==============================
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    // ==============================
    // マテリアル用
    // ==============================
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;


    // ==============================
    // パーティクル用頂点データ
    // ==============================
    struct ParticleVertexData
    {
        Math::Vector4 position;
        Math::Vector2 texcoord;
    };

    // ==============================
    // パーティクル用マテリアル
    // ==============================
    struct ParticleMaterial
    {
        Math::Vector4 color;
        Math::Matrix4x4 uvTransform;
    };

    // ==============================
    // マテリアル書き込み先
    // ==============================
    ParticleMaterial* materialData_ = nullptr;

    // ==============================
    // 内部初期化処理
    // ==============================
    void CreateRootSignature();
    void CreateGraphicsPipeline();
    void CreateVertexResource();
    void CreateMaterialResource();

};
