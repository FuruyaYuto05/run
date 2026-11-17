#include "CollectionScene.h"

#include "AchievementManager.h"
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
constexpr Math::Vector2 kClosedBookSize = { 500.0f, 550.0f };
constexpr Math::Vector2 kOpenBookSize = { 900.0f, 550.0f };
constexpr float kBookEnterDuration = 0.55f;
constexpr float kBookOpenDuration = 0.60f;

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}

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
	TextureManager::GetInstance()->LoadTexture("resources/collection/achievementBookOpen.png");
	TextureManager::GetInstance()->LoadTexture("resources/result/collect1.png");

	backgroundSprite_ = std::make_unique<Sprite>();
	backgroundSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	backgroundSprite_->SetPosition({ 640.0f, 360.0f });
	backgroundSprite_->SetSize({ 1280.0f, 720.0f });
	backgroundSprite_->SetColor({ 0.015f, 0.02f, 0.05f, 1.0f });
	backgroundSprite_->Update();

	bookSprite_ = std::make_unique<Sprite>();
	bookSprite_->Initialize(SpriteCommon::GetInstance(), "resources/collection/achievementBookClosed.png");
	bookSprite_->SetPosition({ 640.0f, -300.0f });
	bookSprite_->SetSize(kClosedBookSize);
	bookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	bookSprite_->Update();

	openBookSprite_ = std::make_unique<Sprite>();
	openBookSprite_->Initialize(SpriteCommon::GetInstance(), "resources/collection/achievementBookOpen.png");
	openBookSprite_->SetPosition({ 640.0f, 360.0f });
	openBookSprite_->SetSize({ 80.0f, kOpenBookSize.y });
	openBookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
	openBookSprite_->Update();

	firstPlayAchievementSprite_ = std::make_unique<Sprite>();
	firstPlayAchievementSprite_->Initialize(SpriteCommon::GetInstance(), "resources/result/collect1.png");
	firstPlayAchievementSprite_->SetPosition({ 405.0f, 285.0f });
	firstPlayAchievementSprite_->SetSize({ 255.0f, 52.0f });
	const bool isFirstPlayUnlocked = AchievementManager::GetInstance().IsUnlocked(AchievementId::FirstPlay);
	firstPlayAchievementSprite_->SetGrayscale(!isFirstPlayUnlocked);
	firstPlayAchievementSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
	firstPlayAchievementSprite_->Update();

	bookState_ = BookState::Entering;
	enterTime_ = 0.0f;
	pulseTime_ = 0.0f;
	openingTime_ = 0.0f;
	previousSpacePressed_ = input_->Pushkey(DIK_SPACE);
}

void CollectionScene::Finalize() {
	firstPlayAchievementSprite_.reset();
	openBookSprite_.reset();
	bookSprite_.reset();
	backgroundSprite_.reset();
}

void CollectionScene::Update() {
	if (input_->Pushkey(DIK_T) || input_->Pushkey(DIK_ESCAPE)) {
		sceneManager_->SetNextScene(std::make_unique<TitleScene>());
		return;
	}

	const bool spacePressed = input_->Pushkey(DIK_SPACE);
	const bool spaceTriggered = spacePressed && !previousSpacePressed_;
	previousSpacePressed_ = spacePressed;

	if (bookState_ == BookState::Entering) {
		// 表紙は最初に画面上から落とす。
		enterTime_ += kDeltaTime;
		const float enterT = std::clamp(enterTime_ / kBookEnterDuration, 0.0f, 1.0f);
		bookSprite_->SetPosition({ 640.0f, -300.0f + (360.0f + 300.0f) * EaseOutBack(enterT) });
		bookSprite_->SetSize(kClosedBookSize);
		if (enterT >= 1.0f) { bookState_ = BookState::Closed; }
	} else if (bookState_ == BookState::Closed) {
		// SPACEを待つ間は表紙を小さく呼吸させる。
		pulseTime_ += kDeltaTime;
		const float scale = 1.0f + std::sin(pulseTime_ * 2.2f) * 0.012f;
		bookSprite_->SetPosition({ 640.0f, 360.0f });
		bookSprite_->SetSize({ kClosedBookSize.x * scale, kClosedBookSize.y * scale });
		if (spaceTriggered) {
			bookState_ = BookState::Opening;
			openingTime_ = 0.0f;
		}
	} else if (bookState_ == BookState::Opening) {
		// 表紙を消しつつ見開きを広げ、幅の揺れでページをぱらぱらさせる。
		openingTime_ += kDeltaTime;
		const float openT = std::clamp(openingTime_ / kBookOpenDuration, 0.0f, 1.0f);
		const float spreadT = SmoothStep(std::clamp((openT - 0.08f) / 0.70f, 0.0f, 1.0f));
		const float closeT = SmoothStep(std::clamp(openT / 0.42f, 0.0f, 1.0f));
		const float pageFlutter = std::sin(openT * 22.0f) * (1.0f - openT) * 35.0f;
		bookSprite_->SetSize({ kClosedBookSize.x * (1.0f - closeT), kClosedBookSize.y });
		bookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f - closeT });
		openBookSprite_->SetSize({ 80.0f + (kOpenBookSize.x - 80.0f) * spreadT + pageFlutter, kOpenBookSize.y });
		openBookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, spreadT });
		if (openT >= 1.0f) { bookState_ = BookState::Open; }
	} else {
		openBookSprite_->SetSize(kOpenBookSize);
		openBookSprite_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	const float achievementT = bookState_ == BookState::Open ? 1.0f :
		(bookState_ == BookState::Opening
			? SmoothStep(std::clamp((openingTime_ - 0.34f) / 0.20f, 0.0f, 1.0f))
			: 0.0f);
	const float achievementScale = 0.88f + achievementT * 0.12f;
	firstPlayAchievementSprite_->SetSize({ 255.0f * achievementScale, 52.0f * achievementScale });
	firstPlayAchievementSprite_->SetColor({ 1.0f, 1.0f, 1.0f, achievementT });
	bookSprite_->Update();
	openBookSprite_->Update();
	firstPlayAchievementSprite_->Update();
}

void CollectionScene::Draw() {
	ID3D12GraphicsCommandList* commandList =
		Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	backgroundSprite_->Draw(commandList);
	openBookSprite_->Draw(commandList);
	bookSprite_->Draw(commandList);
	firstPlayAchievementSprite_->Draw(commandList);
}
