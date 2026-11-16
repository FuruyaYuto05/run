#include "GameOverScene.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "Camera.h"
#include "ModelManager.h"
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
constexpr float kMenuAppearStart = 0.78f;
constexpr float kMenuAppearDuration = 0.38f;
constexpr float kFadeDuration = 0.65f;
constexpr float kMenuWidth = 250.0f;
constexpr float kMenuHeight = 75.0f;
constexpr float kResultOfudaAppearStart = 0.35f;
constexpr float kResultOfudaAppearDuration = 0.32f;
constexpr Math::Vector2 kResultOfudaPosition = { 470.0f, 360.0f };
// 元画像の左右には透明な余白があるため、横を広めにして記録欄を紙の内側へ収める。
// 記録欄の量に合わせ、下側が間延びしない高さにする。
constexpr Math::Vector2 kResultOfudaSize = { 750.0f, 520.0f };
constexpr float kHalfPi = 1.5707963f;
constexpr int kResultDigitCount = 5;
constexpr float kNewRecordAppearStart = 0.85f;
constexpr float kAchievementAppearStart = 1.20f;
constexpr float kToastHoldDuration = 3.0f;
constexpr float kToastFadeDuration = 0.45f;

constexpr std::array<Math::Vector2, 12> kHandPositions = {{
	{ 75.0f, 70.0f }, { 300.0f, 65.0f }, { 980.0f, 65.0f }, { 1210.0f, 90.0f },
	{ 65.0f, 330.0f }, { 1215.0f, 335.0f },
	{ 70.0f, 650.0f }, { 320.0f, 675.0f }, { 960.0f, 675.0f }, { 1210.0f, 650.0f },
	{ 175.0f, 505.0f }, { 1100.0f, 510.0f },
}};

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}

// お札が勢いよく貼り付くような、少し跳ね返る補間。
float EaseOutBack(float t) {
	constexpr float kBack = 1.70158f;
	const float u = t - 1.0f;
	return 1.0f + (kBack + 1.0f) * u * u * u + kBack * u * u;
}
}

#ifdef USE_IMGUI
#include <imgui.h>
#endif

GameOverScene::GameOverScene(int distance, int souls, int score)
	: resultDistance_(distance), resultSouls_(souls), resultScore_(score) {
}
GameOverScene::~GameOverScene() = default;

void GameOverScene::Initialize() {
	// 前シーンの暗転から、祟りを思わせる暗い赤の画面を表示する。
	Object3dCommon::GetInstance()->GetDxCommon()->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	TextureManager::GetInstance()->LoadTexture("resources/result/Retry.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/toTitle.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/resultOfuda.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/resultDistance.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/resultSouls.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/resultScore.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/resultNewRecord.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/collect1.png");
	TextureManager::GetInstance()->LoadTexture("resources/number.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handL.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handR.png");
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");

	backgroundSprite_ = std::make_unique<Sprite>();
	backgroundSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	backgroundSprite_->SetPosition({ 640.0f, 360.0f });
	backgroundSprite_->SetSize({ 1280.0f, 720.0f });
	backgroundSprite_->SetColor({ 0.075f, 0.004f, 0.008f, 1.0f });

	// リザルトの数値や称号を載せるためのお札。表示は貼り付く演出で行う。
	resultOfudaSprite_ = std::make_unique<Sprite>();
	resultOfudaSprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/resultOfuda.png");
	resultOfudaSprite_->SetPosition({ kResultOfudaPosition.x, -260.0f });
	resultOfudaSprite_->SetSize(kResultOfudaSize);
	resultOfudaSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	retrySprite_ = std::make_unique<Sprite>();
	retrySprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/Retry.png");
	// リザルトを見せるため、再挑戦とタイトルへは右側にまとめる。
	retrySprite_->SetPosition({ 1080.0f, 340.0f });

	toTitleSprite_ = std::make_unique<Sprite>();
	toTitleSprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/toTitle.png");
	toTitleSprite_->SetPosition({ 1080.0f, 440.0f });

	// Steam風に、右下へ新記録と実績の通知を順番に表示する。
	newRecordSprite_ = std::make_unique<Sprite>();
	newRecordSprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/resultNewRecord.png");
	newRecordSprite_->SetPosition({ 1070.0f, 575.0f });
	newRecordSprite_->SetSize({ 176.0f, 36.0f });
	newRecordSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	achievementToastSprite_ = std::make_unique<Sprite>();
	achievementToastSprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/collect1.png");
	achievementToastSprite_->SetPosition({ 1070.0f, 635.0f });
	achievementToastSprite_->SetSize({ 220.0f, 45.0f });
	achievementToastSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	fadeSprite_->SetPosition({ 640.0f, 360.0f });
	fadeSprite_->SetSize({ 1280.0f, 720.0f });
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	InitializeHands();
	InitializeResultNumbers();
	InitializeFloatingSouls();
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
	floatingSouls_.clear();
	resultCamera_.reset();
	scoreDigitSprites_.clear();
	soulsDigitSprites_.clear();
	distanceDigitSprites_.clear();
	resultScoreLabelSprite_.reset();
	resultSoulsLabelSprite_.reset();
	resultDistanceLabelSprite_.reset();
	handStamps_.clear();
	fadeSprite_.reset();
	achievementToastSprite_.reset();
	newRecordSprite_.reset();
	toTitleSprite_.reset();
	retrySprite_.reset();
	resultOfudaSprite_.reset();
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
	UpdateFloatingSouls();
	UpdateResultNumbers();
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
	// 魂はお札より奥に見せるため、2Dリザルト画面を描く前に3Dで描画する。
	if (elapsedTime_ >= kResultOfudaAppearStart) {
		for (const FloatingSoul& soul : floatingSouls_) {
			soul.object->Draw();
		}
	}
	SpriteCommon::GetInstance()->PreDraw(commandList);
	backgroundSprite_->Draw(commandList);
	for (const HandStamp& stamp : handStamps_) {
		stamp.sprite->Draw(commandList);
	}
	resultOfudaSprite_->Draw(commandList);
	resultDistanceLabelSprite_->Draw(commandList);
	resultSoulsLabelSprite_->Draw(commandList);
	resultScoreLabelSprite_->Draw(commandList);
	for (const auto& digit : distanceDigitSprites_) { digit->Draw(commandList); }
	for (const auto& digit : soulsDigitSprites_) { digit->Draw(commandList); }
	for (const auto& digit : scoreDigitSprites_) { digit->Draw(commandList); }
	retrySprite_->Draw(commandList);
	toTitleSprite_->Draw(commandList);
	newRecordSprite_->Draw(commandList);
	achievementToastSprite_->Draw(commandList);
	fadeSprite_->Draw(commandList);
}

void GameOverScene::InitializeFloatingSouls() {
	// ゲームオーバー画面用のカメラを作り、UIの周囲で3Dの魂を表示する。
	resultCamera_ = std::make_unique<Camera>();
	resultCamera_->SetTranslate({ 0.0f, 0.0f, -10.0f });
	resultCamera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	Object3dCommon::GetInstance()->SetDefaultCamera(resultCamera_.get());
	resultCamera_->Update();
	ModelManager::GetInstance()->LoadModel("miniSoul.obj");

	constexpr std::array<Math::Vector3, 6> kSoulPositions = {{
		{ -4.6f,  2.9f, 2.0f }, { -4.2f,  0.1f, 2.7f }, { -3.8f, -3.0f, 1.4f },
		{  4.5f,  2.6f, 2.1f }, {  4.2f, -0.2f, 1.5f }, {  3.7f, -3.0f, 2.5f },
	}};
	floatingSouls_.reserve(kSoulPositions.size());
	for (size_t i = 0; i < kSoulPositions.size(); ++i) {
		FloatingSoul soul{};
		soul.object = std::make_unique<Object3d>();
		soul.object->Initialize(Object3dCommon::GetInstance());
		soul.object->SetModel("miniSoul.obj");
		soul.object->SetScale({ 0.38f, 0.38f, 0.38f });
		soul.object->SetCamera(resultCamera_.get());
		soul.basePosition = kSoulPositions[i];
		soul.phase = static_cast<float>(i) * 1.1f;
		soul.object->SetTranslate(soul.basePosition);
		soul.object->SetRotate({ -kHalfPi, soul.phase, 0.0f });
		auto& light = soul.object->GetDirectionalLight();
		light.color = { 0.45f, 0.95f, 1.0f, 1.0f };
		light.intensity = 0.9f;
		light.ambientIntensity = 0.35f;
		soul.object->Update();
		floatingSouls_.push_back(std::move(soul));
	}
}

void GameOverScene::UpdateFloatingSouls() {
	if (!resultCamera_) {
		return;
	}
	resultCamera_->Update();
	for (FloatingSoul& soul : floatingSouls_) {
		Math::Vector3 position = soul.basePosition;
		position.y += std::sin(elapsedTime_ * 2.0f + soul.phase) * 0.24f;
		soul.object->SetTranslate(position);
		soul.object->SetRotate({ -kHalfPi, elapsedTime_ * 0.7f + soul.phase, 0.0f });
		soul.object->Update();
	}
}

void GameOverScene::InitializeResultNumbers() {
	auto makeLabel = [](const char* path, const Math::Vector2& position) {
		auto label = std::make_unique<Sprite>();
		label->Initialize(SpriteCommon::GetInstance(), path);
		label->SetPosition(position);
		label->SetSize({ 132.0f, 27.0f });
		label->Update();
		return label;
	};
	resultDistanceLabelSprite_ = makeLabel("resources/result/resultDistance.png", { 350.0f, 310.0f });
	resultSoulsLabelSprite_ = makeLabel("resources/result/resultSouls.png", { 350.0f, 405.0f });
	resultScoreLabelSprite_ = makeLabel("resources/result/resultScore.png", { 350.0f, 500.0f });

	auto makeDigits = [](std::vector<std::unique_ptr<Sprite>>& digits, float y) {
		digits.reserve(kResultDigitCount);
		for (int i = 0; i < kResultDigitCount; ++i) {
			auto digit = std::make_unique<Sprite>();
			digit->Initialize(SpriteCommon::GetInstance(), "resources/number.png");
			digit->SetPosition({ 515.0f + 34.0f * i, y });
			digit->SetSize({ 34.0f, 34.0f });
			digit->SetTextureSize({ 100.0f, 100.0f });
			digits.push_back(std::move(digit));
		}
	};
	makeDigits(distanceDigitSprites_, 310.0f);
	makeDigits(soulsDigitSprites_, 405.0f);
	makeDigits(scoreDigitSprites_, 500.0f);
	UpdateResultNumbers();
}

void GameOverScene::UpdateResultNumbers() {
	auto updateDigits = [](const std::vector<std::unique_ptr<Sprite>>& digits, int value) {
		int divisor = 10000;
		for (const auto& digit : digits) {
			const int number = (value / divisor) % 10;
			digit->SetTextureLeftTop({ static_cast<float>(number * 100), 0.0f });
			digit->SetTextureSize({ 100.0f, 100.0f });
			digit->Update();
			divisor /= 10;
		}
	};
	updateDigits(distanceDigitSprites_, resultDistance_);
	updateDigits(soulsDigitSprites_, resultSouls_);
	updateDigits(scoreDigitSprites_, resultScore_);
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
	const float resultT = std::clamp(
		(elapsedTime_ - kResultOfudaAppearStart) / kResultOfudaAppearDuration, 0.0f, 1.0f);
	const float resultEased = EaseOutBack(resultT);
	resultOfudaSprite_->SetPosition({
		kResultOfudaPosition.x,
		-260.0f + (kResultOfudaPosition.y + 260.0f) * resultEased,
	});
	resultOfudaSprite_->SetSize({
		kResultOfudaSize.x * (0.92f + resultT * 0.08f),
		kResultOfudaSize.y * (0.92f + resultT * 0.08f),
	});
	resultOfudaSprite_->SetColor({ 1.0f, 1.0f, 1.0f, resultT });

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

	// 通知は少し右から滑り込み、実績を獲得した感覚を出す。
	const float recordT = std::clamp((elapsedTime_ - kNewRecordAppearStart) / 0.22f, 0.0f, 1.0f);
	const float achievementT = std::clamp((elapsedTime_ - kAchievementAppearStart) / 0.22f, 0.0f, 1.0f);
	// Steamの通知のように、少し見せてからフェードアウトする。
	const float recordFade = 1.0f - std::clamp(
		(elapsedTime_ - (kNewRecordAppearStart + kToastHoldDuration)) / kToastFadeDuration, 0.0f, 1.0f);
	const float achievementFade = 1.0f - std::clamp(
		(elapsedTime_ - (kAchievementAppearStart + kToastHoldDuration)) / kToastFadeDuration, 0.0f, 1.0f);
	newRecordSprite_->SetPosition({ 1070.0f + (1.0f - EaseOutBack(recordT)) * 150.0f, 575.0f });
	newRecordSprite_->SetColor({ 1.0f, 1.0f, 1.0f, recordT * recordFade });
	achievementToastSprite_->SetPosition({ 1070.0f + (1.0f - EaseOutBack(achievementT)) * 150.0f, 635.0f });
	achievementToastSprite_->SetColor({ 1.0f, 1.0f, 1.0f, achievementT * achievementFade });

	const float introT = std::clamp(elapsedTime_ / kIntroDuration, 0.0f, 1.0f);
	float fadeAlpha = 1.0f - SmoothStep(introT);
	if (fadingOut_) {
		fadeAlpha = SmoothStep(std::clamp(fadeTime_ / kFadeDuration, 0.0f, 1.0f));
	}
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, fadeAlpha });

	backgroundSprite_->Update();
	resultOfudaSprite_->Update();
	retrySprite_->Update();
	toTitleSprite_->Update();
	newRecordSprite_->Update();
	achievementToastSprite_->Update();
	fadeSprite_->Update();
}

float GameOverScene::NextRandom01() {
	randomState_ = randomState_ * 1664525u + 1013904223u;
	return static_cast<float>((randomState_ >> 8) & 0x00FFFFFFu) / 16777215.0f;
}
