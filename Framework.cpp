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

void Framework::Run() {
	InitializeEngine();
	OnInitialize();

	while (!IsEndRequest()) {
		UpdateEngine();
		if (IsEndRequest()) {
			break;
		}

		OnUpdate();
		if (!IsEndRequest()) {
			OnDraw();
		}
	}

	OnFinalize();
	FinalizeEngine();
}

void Framework::InitializeEngine() {
	winApp_ = std::make_unique<WinApp>(); winApp_->Initialize(); input_ = std::make_unique<Input>(); input_->Initialize(winApp_.get());
	HRESULT result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result));
	sound_ = Sound::GetInstance(); sound_->Initialize();
	dxCommon_ = std::make_unique<DirectXCommon>(); dxCommon_->Initialize(winApp_.get());
	srvManager_ = std::make_unique<SrvManager>(); srvManager_->Initialize(dxCommon_.get());
	imguiManager_ = std::make_unique<ImGuiManager>(); imguiManager_->Initialize(winApp_.get(), dxCommon_.get(), srvManager_.get());
	TextureManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get()); ModelManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get()); ParticleManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());
	ParticleManager::ParticleSettings dustSettings{};
	dustSettings.minScale = { 0.20f, 0.20f, 0.20f };
	dustSettings.maxScale = { 0.38f, 0.38f, 0.38f };
	dustSettings.minVelocity = { -0.85f, 0.04f, -1.05f };
	dustSettings.maxVelocity = { 0.85f, 0.48f, -0.10f };
	dustSettings.startColor = { 0.62f, 0.52f, 0.37f, 0.72f };
	dustSettings.endColor = { 0.42f, 0.34f, 0.25f, 0.0f };
	dustSettings.minLifeTime = 0.35f;
	dustSettings.maxLifeTime = 0.65f;
	ParticleManager::GetInstance()->CreateParticleGroup("dust", "resources/particle/dust.png", dustSettings);

	ParticleManager::ParticleSettings fogSettings{};
	fogSettings.minScale = { 4.4f, 1.10f, 1.0f };
	fogSettings.maxScale = { 7.5f, 1.90f, 1.0f };
	fogSettings.minVelocity = { -0.08f, 0.0f, -1.35f };
	fogSettings.maxVelocity = { 0.08f, 0.04f, -0.65f };
	fogSettings.startColor = { 0.46f, 0.61f, 0.72f, 0.28f };
	fogSettings.endColor = { 0.30f, 0.44f, 0.57f, 0.0f };
	fogSettings.minLifeTime = 5.0f;
	fogSettings.maxLifeTime = 7.5f;
	ParticleManager::GetInstance()->CreateParticleGroup("fog", "resources/particle/fog.png", fogSettings);

	ParticleManager::ParticleSettings soulFireSettings{};
	// 魂の炎は、魂にまとわり付くよう大きめ・短命にする。
	soulFireSettings.minScale = { 0.16f, 0.16f, 0.16f };
	soulFireSettings.maxScale = { 0.27f, 0.27f, 0.27f };
	soulFireSettings.minVelocity = { -0.08f, 0.14f, -0.08f };
	soulFireSettings.maxVelocity = { 0.08f, 0.32f, 0.08f };
	soulFireSettings.startColor = { 0.75f, 0.92f, 1.0f, 0.82f };
	soulFireSettings.endColor = { 0.35f, 0.65f, 1.0f, 0.0f };
	soulFireSettings.minLifeTime = 0.20f;
	soulFireSettings.maxLifeTime = 0.32f;
	ParticleManager::GetInstance()->CreateParticleGroup("soulFire", "resources/particle/soulFire.png", soulFireSettings);
	spriteCommon_ = SpriteCommon::GetInstance(); spriteCommon_->Initialize(dxCommon_.get()); object3dCommon_ = Object3dCommon::GetInstance(); object3dCommon_->Initialize(dxCommon_.get());
}

void Framework::UpdateEngine() { if (winApp_->ProcessMessage()) { endRequest_ = true; return; } input_->Update(); imguiManager_->Begin(); }

void Framework::FinalizeEngine() {
	ParticleManager::GetInstance()->Finalize(); TextureManager::GetInstance()->Finalize(); ModelManager::GetInstance()->Finalize();
	imguiManager_->Finalize(); imguiManager_.reset(); srvManager_.reset(); dxCommon_.reset(); sound_->Finalize(); MFShutdown(); input_.reset(); winApp_->Finalize(); winApp_.reset();
}
