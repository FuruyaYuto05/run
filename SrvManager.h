#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

class DirectXCommon;
// ==============================
// SRV管理クラス
// ==============================
class SrvManager
{
public:
	// ==============================
	// 初期化
	// DirectXCommonのポインタを受け取る
	// ==============================
	void Initialize(DirectXCommon* dxCommon);

	static const uint32_t kMaxSRVCount = 512;

	// ==============================
	// SRVを確保できるかチェック
	// ==============================
	bool CanAllocate() const;

	// ==============================
	// SRV番号を1つ確保する
	// 戻り値：確保したSRV番号
	// ==============================
	uint32_t Allocate();

	// ==============================
	// 指定したSRV番号のCPUハンドルを取得する
	// ==============================
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	// ==============================
	// 指定したSRV番号のGPUハンドルを取得する
	// ==============================
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); }

	// ==============================
	// テクスチャ用SRVを作成する
	// ==============================
	void CreateSRVForTexture2D(
		uint32_t srvIndex,
		ID3D12Resource* resource,
		DXGI_FORMAT format,
		UINT mipLevels
	);

	// ==============================
	// 描画前処理
	// SRV用ディスクリプタヒープをセットする
	// ==============================
	void PreDraw();

	// ==============================
    // 指定したRootParameterにSRVをセットする
    // ==============================
	void SetGraphicsRootDescriptorTable(
		UINT rootParameterIndex,
		uint32_t srvIndex
	);

private:
	// ==============================
	// DirectXCommonを借りる
	// ==============================
	DirectXCommon* directXCommon_ = nullptr;

	// ==============================
	// SRV用ディスクリプタヒープ
	// ==============================
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// ==============================
	// SRVのディスクリプタサイズ
	// ==============================
	uint32_t descriptorSize_ = 0;

	// ==============================
	// 次に使うSRV番号
	// ==============================
	uint32_t useIndex_ = 0;
};
