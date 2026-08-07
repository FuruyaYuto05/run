#include "Game.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"
#include "SrvManager.h"
#include "DirectXCommon.h"
#include <memory>

void Game::Initialize() {
	Framework::Initialize();
	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->SetInput(input_.get());
	sceneManager_->SetNextScene(std::make_unique<TitleScene>());
}
void Game::Finalize() { sceneManager_->Finalize(); sceneManager_ = nullptr; Framework::Finalize(); }
void Game::Update() {
	Framework::Update();
	if (IsEndRequest()) return;
	sceneManager_->Update();
	imguiManager_->End();
}
void Game::Draw() { dxCommon_->PreDraw(); object3dCommon_->SetCommonDrawSetting(); srvManager_->PreDraw(); sceneManager_->Draw(); imguiManager_->Draw(); dxCommon_->PostDraw(); }
