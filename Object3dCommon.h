#pragma once
#include <d3d12.h>
#include <wrl.h>

class DirectXCommon;
class Camera;

// 3Dオブジェクト共通部
class Object3dCommon
{
public:
	static Object3dCommon* GetInstance();
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void SetCommonDrawSetting();

	// ==============================
	// デフォルトカメラを設定する
	// ==============================
	void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }

	// ==============================
	// デフォルトカメラを取得する
	// ==============================
	Camera* GetDefaultCamera() const { return defaultCamera_; }

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkybox_;

	ID3D12PipelineState* GetGraphicsPipelineStateSkybox() const {
		return graphicsPipelineStateSkybox_.Get();
	}

private:
	Object3dCommon() = default;
	Object3dCommon(const Object3dCommon&) = delete;
	Object3dCommon& operator=(const Object3dCommon&) = delete;

	void CreateRootSignature();
	void CreateGraphicsPipeline();

private:
	DirectXCommon* dxCommon_ = nullptr;

	// ==============================
	// 3Dオブジェクトが共通で使うデフォルトカメラ
	// ==============================
	Camera* defaultCamera_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};
