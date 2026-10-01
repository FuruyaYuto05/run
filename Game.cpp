#include "Game.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"
#include "SrvManager.h"
#include "DirectXCommon.h"
#include <memory>

void Game::OnInitialize() {
	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->SetInput(GetInput());
	sceneManager_->SetNextScene(std::make_unique<TitleScene>());
}

void Game::OnFinalize() {
	sceneManager_->Finalize();
	sceneManager_ = nullptr;
}

void Game::OnUpdate() {
	sceneManager_->Update();
	GetImGuiManager()->End();
}

void Game::OnDraw() {
	GetDirectXCommon()->PreDraw();
	GetObject3dCommon()->SetCommonDrawSetting();
	GetSrvManager()->PreDraw();
	sceneManager_->Draw();
	GetImGuiManager()->Draw();
	GetDirectXCommon()->PostDraw();
}
