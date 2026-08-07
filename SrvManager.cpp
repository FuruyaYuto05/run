#include "SrvManager.h"
#include "DirectXCommon.h"

#include <cassert>


// ==============================
// 初期化
// DirectXCommonを受け取って保存する
// ==============================
void SrvManager::Initialize(DirectXCommon* dxCommon)
{
	// ==============================
	// DirectXCommonを保存
	// ==============================
	this->directXCommon_ = dxCommon;

	// ==============================
	// SRV用ディスクリプタヒープを作成
	// ==============================
	descriptorHeap_ = directXCommon_->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		kMaxSRVCount,
		true
	);

	// ==============================
	// SRVの1個分のサイズを取得
	// ==============================
	descriptorSize_ =
		directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	// ==============================
	// 0番から使い始める
	// ==============================
	useIndex_ = 0;
}

// ==============================
// SRVを確保できるかチェック
// ==============================
bool SrvManager::CanAllocate() const
{
	// ==============================
	// useIndex_ が最大数未満なら確保できる
	// ==============================
	return useIndex_ < kMaxSRVCount;
}

// ==============================
// SRV番号を1つ確保する
// ==============================
uint32_t SrvManager::Allocate()
{
	// ==============================
	// 空きがなければ止める
	// ==============================
	assert(CanAllocate());

	// ==============================
	// 今の番号を返すために保存
	// ==============================
	uint32_t index = useIndex_;

	// ==============================
	// 次回は次の番号を使う
	// ==============================
	useIndex_++;

	// ==============================
	// 今回確保した番号を返す
	// ==============================
	return index;
}

// ==============================
// 指定したSRV番号のCPUハンドルを取得する
// ==============================
D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
{
	// ==============================
	// SRVヒープの先頭CPUハンドルを取得
	// ==============================
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU =
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	// ==============================
	// 指定番号ぶん進める
	// ==============================
	handleCPU.ptr += descriptorSize_ * index;

	return handleCPU;
}

// ==============================
// 指定したSRV番号のGPUハンドルを取得する
// ==============================
D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
{
	// ==============================
	// SRVヒープの先頭GPUハンドルを取得
	// ==============================
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU =
		descriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	// ==============================
	// 指定番号ぶん進める
	// ==============================
	handleGPU.ptr += descriptorSize_ * index;

	return handleGPU;
}

// ==============================
// テクスチャ用SRVを作成する
// ==============================
void SrvManager::CreateSRVForTexture2D(
	uint32_t srvIndex,
	ID3D12Resource* resource,
	DXGI_FORMAT format,
	UINT mipLevels
)
{
	// ==============================
	// Texture2D用のSRV設定
	// ==============================
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = mipLevels;

	// ==============================
	// 指定された番号の場所にSRVを作成する
	// ==============================
	directXCommon_->GetDevice()->CreateShaderResourceView(
		resource,
		&srvDesc,
		GetCPUDescriptorHandle(srvIndex)
	);
}

// ==============================
// 描画前処理
// SRV用ディスクリプタヒープをセットする
// ==============================
void SrvManager::PreDraw()
{
	// ==============================
	// SRV用ディスクリプタヒープをコマンドリストにセット
	// ==============================
	ID3D12DescriptorHeap* descriptorHeaps[] = {
		descriptorHeap_.Get()
	};

	directXCommon_->GetCommandList()->SetDescriptorHeaps(
		1,
		descriptorHeaps
	);
}

// ==============================
// 指定したRootParameterにSRVをセットする
// ==============================
void SrvManager::SetGraphicsRootDescriptorTable(
	UINT rootParameterIndex,
	uint32_t srvIndex
)
{
	// ==============================
	// 指定されたSRV番号のGPUハンドルをRootParameterにセットする
	// ==============================
	directXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(
		rootParameterIndex,
		GetGPUDescriptorHandle(srvIndex)
	);
}