#pragma once

// 前方宣言
class DirectXCommon;
class SrvManager;

// 3Dモデル共通部
class ModelCommon
{
public:
    // 初期化
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

    // ゲッター
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    SrvManager* GetSrvManager() const { return srvManager_; }

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
};