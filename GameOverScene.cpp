#include "GameOverScene.h"
#include "GamePlayScene.h"
#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "WinApp.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

GameOverScene::GameOverScene() = default;
GameOverScene::~GameOverScene() = default;

void GameOverScene::Initialize() {
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	backgroundSprite_ = std::make_unique<Sprite>();
	backgroundSprite_->Initialize(SpriteCommon::GetInstance(), "resources/uvChecker.png");
	backgroundSprite_->SetPosition({ WinApp::kClientWidth * 0.5f, WinApp::kClientHeight * 0.5f });
	backgroundSprite_->SetSize({ static_cast<float>(WinApp::kClientWidth), static_cast<float>(WinApp::kClientHeight) });
	backgroundSprite_->SetColor({ 0.35f, 0.03f, 0.03f, 1.0f });
	previousEnterPressed_ = input_->Pushkey(DIK_RETURN);
}

void GameOverScene::Finalize() {
	backgroundSprite_.reset();
}

void GameOverScene::Update() {
	backgroundSprite_->Update();
	const bool enterPressed = input_->Pushkey(DIK_RETURN);
	if (enterPressed && !previousEnterPressed_) {
		sceneManager_->SetNextScene(std::make_unique<TitleScene>());
	}
	previousEnterPressed_ = enterPressed;

#ifdef USE_IMGUI
	ImGui::Begin("Game Over");
	ImGui::Text("GAME OVER");
	ImGui::Text("Press Enter to go to the title screen");
	ImGui::End();
#endif
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	backgroundSprite_->Draw(commandList);
}
