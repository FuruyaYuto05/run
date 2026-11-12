#include "CollectionScene.h"

#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dCommon.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Input.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kBookWidth = 500.0f;
constexpr float kBookHeight = 550.0f;
constexpr float kBookEnterDuration = 0.55f;

float EaseOutBack(float t) {
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;
	const float value = t - 1.0f;
	return 1.0f + c3 * value * value * value + c1 * value * value;
}
}

CollectionScene::CollectionScene() = default;
CollectionScene::~CollectionScene() = default;

void CollectionScene::Initialize() {
	Object3dCommon::GetInstance()->GetDxCommon()->SetClearColor(0.01f, 0.015f, 0.03f, 1.0f);
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");
	TextureManager::GetInstance()->LoadTexture("resources/collection/achievementBookClosed.png");

	backgroundSprite_ = std::make_unique<Sprite>();
	backgroundSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	backgroundSprite_->SetPosition({ 640.0f, 360.0f });
	backgroundSprite_->SetSize({ 1280.0f, 720.0f });
	backgroundSprite_->SetColor({ 0.015f, 0.02f, 0.05f, 1.0f });
	backgroundSprite_->Update();

	bookSprite_ = std::make_unique<Sprite>();
	bookSprite_->Initialize(SpriteCommon::GetInstance(), "resources/collection/achievementBookClosed.png");
	bookSprite_->SetPosition({ 640.0f, -300.0f });
	bookSprite_->SetSize({ kBookWidth, kBookHeight });
	bookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	bookSprite_->Update();
	enterTime_ = 0.0f;
	pulseTime_ = 0.0f;
}

void CollectionScene::Finalize() {
	bookSprite_.reset();
	backgroundSprite_.reset();
}

void CollectionScene::Update() {
	if (input_->Pushkey(DIK_T) || input_->Pushkey(DIK_ESCAPE)) {
		sceneManager_->SetNextScene(std::make_unique<TitleScene>());
		return;
	}

	// 最初は画面上から滑り込み、着地後は少しだけ呼吸するように動かす。
	enterTime_ += kDeltaTime;
	const float enterT = std::clamp(enterTime_ / kBookEnterDuration, 0.0f, 1.0f);
	const float eased = EaseOutBack(enterT);
	const float y = -300.0f + (360.0f + 300.0f) * eased;
	if (enterT >= 1.0f) {
		pulseTime_ += kDeltaTime;
	}
	const float scale = 1.0f + std::sin(pulseTime_ * 2.2f) * 0.012f;
	bookSprite_->SetPosition({ 640.0f, y });
	bookSprite_->SetSize({ kBookWidth * scale, kBookHeight * scale });
	bookSprite_->Update();
}

void CollectionScene::Draw() {
	ID3D12GraphicsCommandList* commandList =
		Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	backgroundSprite_->Draw(commandList);
	bookSprite_->Draw(commandList);
}
