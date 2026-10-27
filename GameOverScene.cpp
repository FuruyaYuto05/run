#include "GameOverScene.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Object3dCommon.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Input.h"
#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kIntroDuration = 0.60f;
constexpr float kTextAppearStart = 0.15f;
constexpr float kTextAppearDuration = 0.45f;
constexpr float kMenuAppearStart = 0.78f;
constexpr float kMenuAppearDuration = 0.38f;
constexpr float kFadeDuration = 0.65f;
constexpr float kGameOverTextWidth = 800.0f;
constexpr float kGameOverTextHeight = 180.0f;
constexpr float kMenuWidth = 320.0f;
constexpr float kMenuHeight = 96.0f;

constexpr std::array<Math::Vector2, 12> kHandPositions = {{
	{ 75.0f, 70.0f }, { 300.0f, 65.0f }, { 980.0f, 65.0f }, { 1210.0f, 90.0f },
	{ 65.0f, 330.0f }, { 1215.0f, 335.0f },
	{ 70.0f, 650.0f }, { 320.0f, 675.0f }, { 960.0f, 675.0f }, { 1210.0f, 650.0f },
	{ 175.0f, 505.0f }, { 1100.0f, 510.0f },
}};

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}
}

#ifdef USE_IMGUI
#include <imgui.h>
#endif

GameOverScene::GameOverScene() = default;
GameOverScene::~GameOverScene() = default;

void GameOverScene::Initialize() {
	// 前シーンの暗転から、祟りを思わせる暗い赤の画面を表示する。
	Object3dCommon::GetInstance()->GetDxCommon()->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	TextureManager::GetInstance()->LoadTexture("resources/GameOver/gameoverText.png");
	TextureManager::GetInstance()->LoadTexture("resources/GameOver/Retry.png");
	TextureManager::GetInstance()->LoadTexture("resources/GameOver/toTitle.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handL.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handR.png");
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");

	backgroundSprite_ = std::make_unique<Sprite>();
	backgroundSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	backgroundSprite_->SetPosition({ 640.0f, 360.0f });
	backgroundSprite_->SetSize({ 1280.0f, 720.0f });
	backgroundSprite_->SetColor({ 0.075f, 0.004f, 0.008f, 1.0f });

	gameOverTextSprite_ = std::make_unique<Sprite>();
	gameOverTextSprite_->Initialize(SpriteCommon::GetInstance(), "resources/GameOver/gameoverText.png");
	gameOverTextSprite_->SetPosition({ 640.0f, 180.0f });

	retrySprite_ = std::make_unique<Sprite>();
	retrySprite_->Initialize(SpriteCommon::GetInstance(), "resources/GameOver/Retry.png");
	retrySprite_->SetPosition({ 640.0f, 475.0f });

	toTitleSprite_ = std::make_unique<Sprite>();
	toTitleSprite_->Initialize(SpriteCommon::GetInstance(), "resources/GameOver/toTitle.png");
	toTitleSprite_->SetPosition({ 640.0f, 595.0f });

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	fadeSprite_->SetPosition({ 640.0f, 360.0f });
	fadeSprite_->SetSize({ 1280.0f, 720.0f });
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	InitializeHands();
	elapsedTime_ = 0.0f;
	fadeTime_ = 0.0f;
	selectedMenu_ = MenuItem::Retry;
	menuReady_ = false;
	fadingOut_ = false;
	previousEnterPressed_ = input_->Pushkey(DIK_RETURN);
	previousUpPressed_ = input_->Pushkey(DIK_UP) || input_->Pushkey(DIK_W);
	previousDownPressed_ = input_->Pushkey(DIK_DOWN) || input_->Pushkey(DIK_S);
	UpdateLayout();
}

void GameOverScene::Finalize() {
	handStamps_.clear();
	fadeSprite_.reset();
	toTitleSprite_.reset();
	retrySprite_.reset();
	gameOverTextSprite_.reset();
	backgroundSprite_.reset();
}

void GameOverScene::Update() {
	elapsedTime_ += kDeltaTime;
	menuReady_ = elapsedTime_ >= kMenuAppearStart + kMenuAppearDuration;

	const bool enterPressed = input_->Pushkey(DIK_RETURN);
	const bool upPressed = input_->Pushkey(DIK_UP) || input_->Pushkey(DIK_W);
	const bool downPressed = input_->Pushkey(DIK_DOWN) || input_->Pushkey(DIK_S);
	const bool enterTriggered = enterPressed && !previousEnterPressed_;
	const bool upTriggered = upPressed && !previousUpPressed_;
	const bool downTriggered = downPressed && !previousDownPressed_;
	previousEnterPressed_ = enterPressed;
	previousUpPressed_ = upPressed;
	previousDownPressed_ = downPressed;

	if (!fadingOut_ && menuReady_) {
		if (upTriggered || downTriggered) {
			selectedMenu_ = selectedMenu_ == MenuItem::Retry ? MenuItem::Title : MenuItem::Retry;
		}
		if (enterTriggered) {
			fadingOut_ = true;
			fadeTime_ = 0.0f;
		}
	}

	if (fadingOut_) {
		fadeTime_ += kDeltaTime;
		if (fadeTime_ >= kFadeDuration) {
			if (selectedMenu_ == MenuItem::Retry) {
				sceneManager_->SetNextScene(std::make_unique<GamePlayScene>());
			} else {
				sceneManager_->SetNextScene(std::make_unique<TitleScene>());
			}
		}
	}

	UpdateHands();
	UpdateLayout();

#ifdef USE_IMGUI
	ImGui::Begin("Game Over");
	ImGui::Text("Selected: %s", selectedMenu_ == MenuItem::Retry ? "RETRY" : "TITLE");
	ImGui::Text("Menu Input: %s", menuReady_ ? "Ready" : "Waiting for intro");
	ImGui::End();
#endif
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList =
		Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	backgroundSprite_->Draw(commandList);
	for (const HandStamp& stamp : handStamps_) {
		stamp.sprite->Draw(commandList);
	}
	gameOverTextSprite_->Draw(commandList);
	retrySprite_->Draw(commandList);
	toTitleSprite_->Draw(commandList);
	fadeSprite_->Draw(commandList);
}

void GameOverScene::InitializeHands() {
	handStamps_.clear();
	handStamps_.reserve(kHandPositions.size());
	for (size_t i = 0; i < kHandPositions.size(); ++i) {
		HandStamp stamp{};
		stamp.sprite = std::make_unique<Sprite>();
		stamp.sprite->Initialize(
			SpriteCommon::GetInstance(),
			NextRandom01() < 0.5f ? "resources/scene/handL.png" : "resources/scene/handR.png");
		const float size = 250.0f + NextRandom01() * 170.0f;
		stamp.baseSize = { size, size };
		stamp.appearTime = 0.08f + static_cast<float>(i) * 0.045f;
		stamp.sprite->SetPosition({
			kHandPositions[i].x + (NextRandom01() - 0.5f) * 75.0f,
			kHandPositions[i].y + (NextRandom01() - 0.5f) * 55.0f,
		});
		stamp.sprite->SetRotation({ 0.0f, 0.0f, -0.8f + NextRandom01() * 1.6f });
		stamp.sprite->SetSize({ size * 0.70f, size * 0.70f });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
		stamp.sprite->Update();
		handStamps_.push_back(std::move(stamp));
	}
}

void GameOverScene::UpdateHands() {
	constexpr float kPopDuration = 0.16f;
	for (HandStamp& stamp : handStamps_) {
		const float t = std::clamp(
			(elapsedTime_ - stamp.appearTime) / kPopDuration, 0.0f, 1.0f);
		const float eased = SmoothStep(t);
		const float scale = 0.70f + eased * 0.30f;
		stamp.sprite->SetSize({ stamp.baseSize.x * scale, stamp.baseSize.y * scale });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, eased * 0.75f });
		stamp.sprite->Update();
	}
}

void GameOverScene::UpdateLayout() {
	const float textT = std::clamp(
		(elapsedTime_ - kTextAppearStart) / kTextAppearDuration, 0.0f, 1.0f);
	const float textEased = SmoothStep(textT);
	const float textScale = 0.78f + textEased * 0.22f;
	gameOverTextSprite_->SetSize({
		kGameOverTextWidth * textScale,
		kGameOverTextHeight * textScale,
	});
	gameOverTextSprite_->SetColor({ 1.0f, 1.0f, 1.0f, textEased });

	const float menuT = std::clamp(
		(elapsedTime_ - kMenuAppearStart) / kMenuAppearDuration, 0.0f, 1.0f);
	const float menuAlpha = SmoothStep(menuT);
	const float pulse = 1.0f + std::sin(elapsedTime_ * 6.0f) * 0.045f;
	const float retryScale = menuReady_ && selectedMenu_ == MenuItem::Retry ? pulse : 0.88f;
	const float titleScale = menuReady_ && selectedMenu_ == MenuItem::Title ? pulse : 0.88f;
	retrySprite_->SetSize({ kMenuWidth * retryScale, kMenuHeight * retryScale });
	toTitleSprite_->SetSize({ kMenuWidth * titleScale, kMenuHeight * titleScale });
	retrySprite_->SetColor({ 1.0f, 1.0f, 1.0f, menuAlpha * (selectedMenu_ == MenuItem::Retry ? 1.0f : 0.55f) });
	toTitleSprite_->SetColor({ 1.0f, 1.0f, 1.0f, menuAlpha * (selectedMenu_ == MenuItem::Title ? 1.0f : 0.55f) });

	const float introT = std::clamp(elapsedTime_ / kIntroDuration, 0.0f, 1.0f);
	float fadeAlpha = 1.0f - SmoothStep(introT);
	if (fadingOut_) {
		fadeAlpha = SmoothStep(std::clamp(fadeTime_ / kFadeDuration, 0.0f, 1.0f));
	}
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, fadeAlpha });

	backgroundSprite_->Update();
	gameOverTextSprite_->Update();
	retrySprite_->Update();
	toTitleSprite_->Update();
	fadeSprite_->Update();
}

float GameOverScene::NextRandom01() {
	randomState_ = randomState_ * 1664525u + 1013904223u;
	return static_cast<float>((randomState_ >> 8) & 0x00FFFFFFu) / 16777215.0f;
}
