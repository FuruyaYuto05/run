#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"
#include <unordered_map>


class DirectXCommon;
class SrvManager;

class TextureManager
{
private:
    TextureManager() = default;
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // ★ SRVインデックスの開始番号（ImGuiが0番を使うので1から）
    static uint32_t kSRVIndexTop;

    SrvManager* srvManager_ = nullptr;
    DirectXCommon* dxCommon_ = nullptr;

    struct TextureData {
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        uint32_t srvIndex = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
    };

    std::unordered_map<std::string, TextureData> textureDatas;
public:
    static TextureManager* GetInstance();
    void SetDirectXCommon(DirectXCommon* dxCommon);

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
    void Finalize();

    void LoadTexture(const std::string& filePath);

    // ==============================
    // メタデータを取得
    // ==============================
    const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

    // ==============================
    // SRVインデックスを取得
    // ==============================
    uint32_t GetTextureIndexByFilePath(const std::string& filePath);

    // ==============================
    // GPUハンドルを取得
    // ==============================
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
};
