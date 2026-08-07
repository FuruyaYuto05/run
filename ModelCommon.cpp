#include "ModelCommon.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

void ModelCommon::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    // ==============================
    // DirectXCommonを保存
    // ==============================
    dxCommon_ = dxCommon;

    // ==============================
    // SrvManagerを保存
    // ==============================
    srvManager_ = srvManager;
}