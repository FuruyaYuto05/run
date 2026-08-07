#include "TitleScene.h"
#include "GamePlayScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "DirectXCommon.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include <imgui.h>
#include <memory>

TitleScene::TitleScene() = default;
TitleScene::~TitleScene() = default;

void TitleScene::Initialize() {
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize(SpriteCommon::GetInstance(), "resources/uvChecker.png");
	titleSprite_->SetPosition({ 640.0f, 360.0f });
	titleSprite_->SetSize({ 320.0f, 180.0f });
}
void TitleScene::Finalize() { titleSprite_.reset(); }
void TitleScene::Update() {
	titleSprite_->Update();

	// Enterキーでゲームプレイシーンへの切り替えを予約する
	if (input_->Pushkey(DIK_RETURN)) {
		sceneManager_->SetNextScene(std::make_unique<GamePlayScene>());
	}
#ifdef USE_IMGUI
	ImGui::Begin("Title Scene");
	ImGui::Text("TITLE");
	ImGui::Text("GamePlayScene is not running.");
	ImGui::End();
#endif
}
void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	titleSprite_->Draw(commandList);
}
