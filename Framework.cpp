#include "Framework.h"
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include <cassert>
#include <mfapi.h>
#pragma comment(lib, "mfplat.lib")
Framework::Framework() = default;
Framework::~Framework() = default;
void Framework::Run() { Initialize(); while (!IsEndRequest()) { Update(); if (!IsEndRequest()) { Draw(); } } Finalize(); }
void Framework::Initialize() {
	winApp_ = std::make_unique<WinApp>(); winApp_->Initialize(); input_ = std::make_unique<Input>(); input_->Initialize(winApp_.get());
	HRESULT result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result));
	sound_ = Sound::GetInstance(); sound_->Initialize();
	dxCommon_ = std::make_unique<DirectXCommon>(); dxCommon_->Initialize(winApp_.get());
	srvManager_ = std::make_unique<SrvManager>(); srvManager_->Initialize(dxCommon_.get());
	imguiManager_ = std::make_unique<ImGuiManager>(); imguiManager_->Initialize(winApp_.get(), dxCommon_.get(), srvManager_.get());
	TextureManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get()); ModelManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get()); ParticleManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());
	spriteCommon_ = SpriteCommon::GetInstance(); spriteCommon_->Initialize(dxCommon_.get()); object3dCommon_ = Object3dCommon::GetInstance(); object3dCommon_->Initialize(dxCommon_.get());
}
void Framework::Update() { if (winApp_->ProcessMessage()) { endRequest_ = true; return; } input_->Update(); imguiManager_->Begin(); }
void Framework::Finalize() {
	ParticleManager::GetInstance()->Finalize(); TextureManager::GetInstance()->Finalize(); ModelManager::GetInstance()->Finalize();
	imguiManager_->Finalize(); imguiManager_.reset(); srvManager_.reset(); dxCommon_.reset(); sound_->Finalize(); MFShutdown(); input_.reset(); winApp_->Finalize(); winApp_.reset();
}
