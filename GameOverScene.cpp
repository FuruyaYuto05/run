#include "GameOverScene.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dCommon.h"
#include "DirectXCommon.h"

namespace {
constexpr float kReturnToTitleTime = 0.75f;
constexpr float kDeltaTime = 1.0f / 60.0f;
}

#ifdef USE_IMGUI
#include <imgui.h>
#endif

GameOverScene::GameOverScene() = default;
GameOverScene::~GameOverScene() = default;

void GameOverScene::Initialize() {
	// UV画像は使わず、短い黒画面を挟んでタイトルへ戻す。
	Object3dCommon::GetInstance()->GetDxCommon()->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	elapsedTime_ = 0.0f;
}

void GameOverScene::Finalize() {
}

void GameOverScene::Update() {
	elapsedTime_ += kDeltaTime;
	if (elapsedTime_ >= kReturnToTitleTime) {
		sceneManager_->SetNextScene(std::make_unique<TitleScene>());
	}

#ifdef USE_IMGUI
	ImGui::Begin("Game Over");
	ImGui::Text("Returning to title...");
	ImGui::End();
#endif
}

void GameOverScene::Draw() {
	// 背景のクリアカラーだけで黒画面を表示する。
}
