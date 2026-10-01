#include "TitleScene.h"
#include "GamePlayScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "DirectXCommon.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include <algorithm>
#include <cmath>
#include <memory>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kIntroDuration = 1.0f;
constexpr float kExitDuration = 0.45f;
constexpr float kLogoX = 640.0f;
constexpr float kLogoY = 245.0f;
constexpr float kLogoWidth = 704.0f;
constexpr float kLogoHeight = 120.0f;

float EaseOutBack(float t) {
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;
	const float value = t - 1.0f;
	return 1.0f + c3 * value * value * value + c1 * value * value;
}

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}
}

TitleScene::TitleScene() = default;
TitleScene::~TitleScene() = default;

void TitleScene::Initialize() {
	TextureManager::GetInstance()->LoadTexture("resources/title.png");

	shadowSprite_ = std::make_unique<Sprite>();
	shadowSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title.png");

	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title.png");

	phase_ = Phase::Intro;
	phaseTime_ = 0.0f;
	totalTime_ = 0.0f;
	previousEnterPressed_ = input_->Pushkey(DIK_RETURN);
	ApplyLogoTransform(0.1f, 80.0f, -0.12f, 0.0f);
}

void TitleScene::Finalize() {
	titleSprite_.reset();
	shadowSprite_.reset();
}

void TitleScene::Update() {
	phaseTime_ += kDeltaTime;
	totalTime_ += kDeltaTime;

	const bool enterPressed = input_->Pushkey(DIK_RETURN);
	const bool enterTriggered = enterPressed && !previousEnterPressed_;
	previousEnterPressed_ = enterPressed;

	switch (phase_) {
	case Phase::Intro: {
		const float t = std::clamp(phaseTime_ / kIntroDuration, 0.0f, 1.0f);
		const float eased = EaseOutBack(t);
		const float scale = 0.1f + 0.9f * eased;
		const float yOffset = 80.0f * (1.0f - eased);
		const float rotation = -0.12f * (1.0f - t);
		ApplyLogoTransform(scale, yOffset, rotation, SmoothStep(t));
		if (t >= 1.0f) {
			phase_ = Phase::Idle;
			phaseTime_ = 0.0f;
		}
		break;
	}
	case Phase::Idle: {
		const float wave = std::sin(totalTime_ * 2.8f);
		const float scale = 1.0f + wave * 0.012f;
		const float yOffset = wave * 6.0f;
		const float rotation = std::sin(totalTime_ * 1.7f) * 0.012f;
		ApplyLogoTransform(scale, yOffset, rotation, 1.0f);
		break;
	}
	case Phase::Exit: {
		const float t = std::clamp(phaseTime_ / kExitDuration, 0.0f, 1.0f);
		const float eased = SmoothStep(t);
		ApplyLogoTransform(1.0f + eased * 0.15f, -80.0f * eased, 0.0f, 1.0f - eased);
		if (t >= 1.0f) {
			sceneManager_->SetNextScene(std::make_unique<GamePlayScene>());
		}
		break;
	}
	}

	if (enterTriggered && phase_ != Phase::Exit) {
		phase_ = Phase::Exit;
		phaseTime_ = 0.0f;
	}

	shadowSprite_->Update();
	titleSprite_->Update();

#ifdef USE_IMGUI
	ImGui::Begin("Title Scene");
	ImGui::Text("Press Enter to Start");
	ImGui::Text("Animation Time: %.2f", totalTime_);
	ImGui::End();
#endif
}

void TitleScene::ApplyLogoTransform(float scale, float yOffset, float rotation, float alpha) {
	const Math::Vector2 size = { kLogoWidth * scale, kLogoHeight * scale };
	const Math::Vector2 position = { kLogoX, kLogoY + yOffset };

	titleSprite_->SetPosition(position);
	titleSprite_->SetSize(size);
	titleSprite_->SetRotation({ 0.0f, 0.0f, rotation });
	titleSprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha });

	shadowSprite_->SetPosition({ position.x + 8.0f, position.y + 8.0f });
	shadowSprite_->SetSize(size);
	shadowSprite_->SetRotation({ 0.0f, 0.0f, rotation });
	shadowSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha * 0.35f });
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	shadowSprite_->Draw(commandList);
	titleSprite_->Draw(commandList);
}
