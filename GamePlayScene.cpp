#include "GamePlayScene.h"
#include "Camera.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Player.h"
#include "CourseManager.h"
#include "ObstacleManager.h"
#include "GameOverScene.h"
#include "SceneManager.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <random>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kOpeningHoldDuration = 0.85f;
constexpr float kOpeningMoveDuration = 2.80f;
constexpr float kPi = 3.1415927f;
constexpr Math::Vector3 kOpeningCameraPosition = { 0.0f, 2.4f, 5.5f };
constexpr Math::Vector3 kOpeningCameraRotation = { 0.266f, kPi, 0.0f };
constexpr Math::Vector3 kOpeningControlPoint1 = { 4.0f, 2.7f, 3.0f };
constexpr Math::Vector3 kOpeningControlPoint2 = { 5.0f, 3.8f, -5.5f };
constexpr Math::Vector3 kGameplayCameraPosition = { 0.0f, 4.0f, -10.0f };
constexpr Math::Vector3 kGameplayCameraRotation = { 0.3f, 0.0f, 0.0f };
constexpr Math::Vector3 kGameplayCameraRotationForOpening = { 0.3f, kPi * 2.0f, 0.0f };
constexpr Math::Vector3 kGhostOpeningPosition = { 0.0f, 0.25f, -4.25f };
constexpr Math::Vector3 kGhostHiddenPosition = { 0.0f, 0.25f, -13.0f };
constexpr Math::Vector3 kGhostScale = { 0.70f, 0.70f, 0.70f };

struct StartRunLayout {
	Math::Vector2 position;
	Math::Vector2 size;
	float rotation;
};

// 自機が映る画面中央を避け、周囲を囲むように配置する。
constexpr std::array<StartRunLayout, 10> kStartRunLayouts = {{
	{{ 170.0f, 95.0f }, { 245.0f, 46.0f }, -0.08f},
	{{1080.0f, 105.0f }, { 225.0f, 42.0f },  0.06f},
	{{ 165.0f, 225.0f }, { 215.0f, 40.0f },  0.04f},
	{{1110.0f, 240.0f }, { 250.0f, 47.0f }, -0.07f},
	{{ 145.0f, 365.0f }, { 235.0f, 44.0f }, -0.03f},
	{{1135.0f, 390.0f }, { 220.0f, 41.0f },  0.08f},
	{{ 185.0f, 515.0f }, { 260.0f, 49.0f },  0.07f},
	{{1090.0f, 535.0f }, { 245.0f, 46.0f }, -0.05f},
	{{ 355.0f, 655.0f }, { 230.0f, 43.0f }, -0.06f},
	{{ 920.0f, 650.0f }, { 270.0f, 51.0f },  0.05f},
}};

constexpr float kStartRunFirstAppearTime = 0.65f;
constexpr float kStartRunAppearInterval = 0.25f;
constexpr float kStartRunPopDuration = 0.12f;
constexpr float kStartRunFadeDuration = 0.60f;
constexpr float kStartRunSizeScale = 1.65f;
// 提出用：操作開始後、この秒数でゲームオーバー演出を確認する。
constexpr float kAutoGameOverDelay = 3.0f;
constexpr float kDeathImpactDuration = 0.28f;
constexpr float kDeathHoldDuration = 0.45f;
constexpr float kDeathHandCoverDuration = 0.85f;
constexpr float kDeathFadeDuration = 0.65f;
constexpr int kDeathHandColumns = 6;
constexpr int kDeathHandRows = 4;
constexpr int kDeathHandCount = kDeathHandColumns * kDeathHandRows;

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}

Math::Vector3 LerpVector3(const Math::Vector3& start, const Math::Vector3& end, float t) {
	return {
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t,
		start.z + (end.z - start.z) * t,
	};
}

Math::Vector3 CubicBezierVector3(
	const Math::Vector3& start,
	const Math::Vector3& control1,
	const Math::Vector3& control2,
	const Math::Vector3& end,
	float t) {
	const float oneMinusT = 1.0f - t;
	const float startWeight = oneMinusT * oneMinusT * oneMinusT;
	const float control1Weight = 3.0f * oneMinusT * oneMinusT * t;
	const float control2Weight = 3.0f * oneMinusT * t * t;
	const float endWeight = t * t * t;
	return {
		start.x * startWeight + control1.x * control1Weight + control2.x * control2Weight + end.x * endWeight,
		start.y * startWeight + control1.y * control1Weight + control2.y * control2Weight + end.y * endWeight,
		start.z * startWeight + control1.z * control1Weight + control2.z * control2Weight + end.z * endWeight,
	};
}

}

GamePlayScene::GamePlayScene() = default;
GamePlayScene::~GamePlayScene() = default;

void GamePlayScene::Initialize() {
	// ゲームで使用する画像・カメラを準備する。
	object3dCommon_ = Object3dCommon::GetInstance();
	// 仮背景として、祟りの雰囲気に合う暗い夜色にする。
	object3dCommon_->GetDxCommon()->SetClearColor(0.015f, 0.02f, 0.04f, 1.0f);
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/life.png");
	TextureManager::GetInstance()->LoadTexture("resources/startRun.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handL.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handR.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");
	camera_ = std::make_unique<Camera>();
	// 最初はプレイヤーの顔を正面から映す。
	camera_->SetRotate(kOpeningCameraRotation);
	camera_->SetTranslate(kOpeningCameraPosition);
	object3dCommon_->SetDefaultCamera(camera_.get());
	camera_->Update();
	// プレイヤーの残りHPを表示するスプライトを作る。
	for (uint32_t i = 0; i < 3; ++i) {
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(SpriteCommon::GetInstance(), "resources/life.png");
		sprite->SetPosition({ 35.0f + 50.0f * i, 50.0f });
		sprite->SetSize({ 36.0f, 72.0f });
		sprite->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		hpSprites_.push_back(std::move(sprite));
	}
	// プレイヤーとコース、障害物に必要なモデルを読み込む。
	ModelManager::GetInstance()->LoadModel("human/Mannequin_Large.glb");
	ModelManager::GetInstance()->LoadModel("Ghost.gltf");
	ModelManager::GetInstance()->LoadModel("road.obj");
	ModelManager::GetInstance()->LoadModel("tyoutin.obj");
	animatedModel_ = ModelManager::GetInstance()->FindModel("human/Mannequin_Large.glb");
	ghostModel_ = ModelManager::GetInstance()->FindModel("Ghost.gltf");
	// ゲーム内の各管理オブジェクトを生成する。
	player_ = std::make_unique<Player>();
	player_->Initialize(object3dCommon_, input_);
	// スタート演出で、自機の後方から追ってくる祟りを表示する。
	ghostObject_ = std::make_unique<Object3d>();
	ghostObject_->Initialize(object3dCommon_);
	ghostObject_->SetModel("Ghost.gltf");
	ghostObject_->SetScale(kGhostScale);
	ghostObject_->SetRotate({ 0.0f, 0.0f, 0.0f });
	ghostObject_->SetTranslate(kGhostOpeningPosition);
	courseManager_ = std::make_unique<CourseManager>();
	courseManager_->Initialize(object3dCommon_);
	obstacleManager_ = std::make_unique<ObstacleManager>();
	obstacleManager_->Initialize(object3dCommon_);
	animation_ = LoadAnimationFile("resources/human", "Rig_Large_MovementBasic.glb");
	deathAnimation_ = LoadAnimationFile("resources/human", "Standing Death Forward 01.fbx");
	ghostAnimation_ = LoadAnimationFile("resources", "Ghost.gltf", "Fast_Flying");
	// 演出中に描画できるよう、各3Dオブジェクトの行列だけ先に作る。
	player_->Update();
	ghostObject_->Update();
	courseManager_->UpdateTransformOnly();
	obstacleManager_->Update(kGameplayCameraPosition.z, 0.0f);
	openingCameraTime_ = 0.0f;
	gameplayTime_ = 0.0f;
	gameplayStarted_ = false;
	InitializeStartRunEffect();
	InitializeSceneReveal();
	InitializeGameOverEffect();
}

void GamePlayScene::Finalize() {
	// シーンで生成したオブジェクトと音声を解放する。
	player_->Finalize();
	courseManager_->Finalize();
	obstacleManager_->Finalize();
	player_.reset();
	ghostObject_.reset();
	ghostModel_ = nullptr;
	courseManager_.reset();
	obstacleManager_.reset();
	hpSprites_.clear();
	startRunSprites_.clear();
	startRunAppearRanks_.clear();
	revealLeftSprite_.reset();
	revealRightSprite_.reset();
	deathHandStamps_.clear();
	gameOverTintSprite_.reset();
	gameOverFadeSprite_.reset();
	camera_.reset();
}

void GamePlayScene::Update() {
#ifdef USE_IMGUI
	// デバッグ中だけ各種パラメータを画面から調整する。
	player_->DrawImGui();
	courseManager_->DrawImGui();

	ImGui::Begin("Camera Settings");
	Math::Vector3 cameraPosition = camera_->GetTranslate();
	if (ImGui::DragFloat3("Position", &cameraPosition.x, 0.1f)) { camera_->SetTranslate(cameraPosition); }
	Math::Vector3 cameraRotation = camera_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &cameraRotation.x, 0.01f)) { camera_->SetRotate(cameraRotation); }
	ImGui::End();
#endif
	if (gameOverPhase_ != GameOverPhase::None) {
		UpdateGameOverSequence();
		return;
	}
	// 走行アニメーションを繰り返し再生する。
	if (animatedModel_ && animation_.duration > 0) { animationTime_ = std::fmod(animationTime_ + kDeltaTime, animation_.duration); animatedModel_->UpdateSkeleton(animation_, animationTime_); }
	// 祟りの飛行モーションを繰り返し再生する。
	if (ghostModel_ && ghostAnimation_.duration > 0.0f) {
		ghostAnimationTime_ = std::fmod(ghostAnimationTime_ + kDeltaTime, ghostAnimation_.duration);
		ghostModel_->UpdateSkeleton(ghostAnimation_, ghostAnimationTime_);
	}

	// カメラ演出が終わるまではコースと障害物を動かさない。
	if (!gameplayStarted_) {
		UpdateOpeningCamera();
		UpdateStartRunEffect();
		camera_->Update();
		// 自機は移動させず、移動中のカメラだけを描画へ反映する。
		player_->UpdateTransformOnly();
		UpdateGhost();
		// 床は流すが、演出中は再配置を止めて画面内で消えるのを防ぐ。
		courseManager_->Update(camera_->GetTranslate().z, false);
		for (const auto& sprite : hpSprites_) { sprite->Update(); }
		UpdateSceneReveal();
		return;
	}

	// カメラ・プレイヤー・コース・障害物を順番に更新する。
	camera_->Update();
	player_->Update();
	UpdateGhost();
	courseManager_->Update(camera_->GetTranslate().z);
	obstacleManager_->Update(camera_->GetTranslate().z, courseManager_->GetMoveSpeed());
	// 提出時に一連のゲームオーバー演出を確実に確認できるよう自動で開始する。
	gameplayTime_ += kDeltaTime;
	if (gameplayTime_ >= kAutoGameOverDelay) {
		StartGameOverSequence();
		return;
	}
	// 無敵中でなければ障害物との当たり判定を行う。
	if (!player_->IsInvincible() && obstacleManager_->CheckCollision(player_->GetPosition())) {
		player_->OnCollision();
		if (player_->IsDead()) {
			StartGameOverSequence();
			return;
		}
	}
	// HP表示とゲーム開始時の画面開放演出を更新する。
	for (const auto& sprite : hpSprites_) { sprite->Update(); }
	UpdateStartRunEffect();
	UpdateSceneReveal();
}

void GamePlayScene::UpdateGhost() {
	if (!ghostObject_) {
		return;
	}

	// 正面カメラでは自機の後ろに見せ、カメラが回り込む間に後方へ下げる。
	// 操作開始後はカメラより後ろにいるため、通常プレイの視界を塞がない。
	const float moveTime = openingCameraTime_ - kOpeningHoldDuration;
	const float t = std::clamp(moveTime / kOpeningMoveDuration, 0.0f, 1.0f);
	const float eased = SmoothStep(t);
	Math::Vector3 position = LerpVector3(kGhostOpeningPosition, kGhostHiddenPosition, eased);
	position.y += std::sin(ghostAnimationTime_ * 3.0f) * 0.08f;
	ghostObject_->SetTranslate(position);
	ghostObject_->Update();
}

void GamePlayScene::UpdateOpeningCamera() {
	openingCameraTime_ += kDeltaTime;

	// 正面から右側を回り、後方まで1本の曲線で止まらずに移動する。
	const float moveTime = openingCameraTime_ - kOpeningHoldDuration;
	const float t = std::clamp(moveTime / kOpeningMoveDuration, 0.0f, 1.0f);
	const float eased = SmoothStep(t);
	const Math::Vector3 cameraPosition = CubicBezierVector3(
		kOpeningCameraPosition,
		kOpeningControlPoint1,
		kOpeningControlPoint2,
		kGameplayCameraPosition,
		eased);
	const Math::Vector3 cameraRotation = LerpVector3(
		kOpeningCameraRotation,
		kGameplayCameraRotationForOpening,
		eased);
	camera_->SetTranslate(cameraPosition);
	camera_->SetRotate(cameraRotation);

	if (t >= 1.0f) {
		camera_->SetTranslate(kGameplayCameraPosition);
		camera_->SetRotate(kGameplayCameraRotation);
		gameplayStarted_ = true;
	}
}

void GamePlayScene::InitializeStartRunEffect() {
	startRunSprites_.clear();
	startRunSprites_.reserve(kStartRunLayouts.size());
	startRunAppearRanks_.assign(kStartRunLayouts.size(), 0);

	// 同じ配置でも、出現する順番はゲーム開始ごとに変える。
	std::vector<size_t> appearOrder(kStartRunLayouts.size());
	std::iota(appearOrder.begin(), appearOrder.end(), size_t{0});
	static std::mt19937 randomEngine(std::random_device{}());
	std::shuffle(appearOrder.begin(), appearOrder.end(), randomEngine);
	for (size_t rank = 0; rank < appearOrder.size(); ++rank) {
		startRunAppearRanks_[appearOrder[rank]] = rank;
	}

	for (const StartRunLayout& layout : kStartRunLayouts) {
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(SpriteCommon::GetInstance(), "resources/startRun.png");
		sprite->SetPosition(layout.position);
		sprite->SetSize(layout.size);
		sprite->SetRotation({ 0.0f, 0.0f, layout.rotation });
		sprite->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
		sprite->Update();
		startRunSprites_.push_back(std::move(sprite));
	}

	startRunFadeTime_ = 0.0f;
	startRunEffectActive_ = true;
}

void GamePlayScene::UpdateStartRunEffect() {
	if (!startRunEffectActive_) {
		return;
	}

	float fadeAlpha = 1.0f;
	if (gameplayStarted_) {
		startRunFadeTime_ += kDeltaTime;
		fadeAlpha = 1.0f - std::clamp(
			startRunFadeTime_ / kStartRunFadeDuration, 0.0f, 1.0f);
	}

	const float effectTime = openingCameraTime_ + startRunFadeTime_;
	for (size_t i = 0; i < startRunSprites_.size(); ++i) {
		const StartRunLayout& layout = kStartRunLayouts[i];
		const float appearTime = kStartRunFirstAppearTime +
			kStartRunAppearInterval * static_cast<float>(startRunAppearRanks_[i]);
		const float age = openingCameraTime_ - appearTime;
		const float popT = std::clamp(age / kStartRunPopDuration, 0.0f, 1.0f);
		const float appearAlpha = age >= 0.0f ? SmoothStep(popT) : 0.0f;
		const float pulse = 1.0f + 0.025f *
			std::sin(effectTime * 8.0f + static_cast<float>(i) * 1.7f);
		const float popScale = (0.80f + 0.20f * SmoothStep(popT)) * pulse;
		const float shakeX = 2.0f *
			std::sin(effectTime * 11.0f + static_cast<float>(i));
		const float shakeY = 1.5f *
			std::cos(effectTime * 9.0f + static_cast<float>(i) * 1.3f);

		Sprite* sprite = startRunSprites_[i].get();
		sprite->SetPosition({
			layout.position.x + shakeX,
			layout.position.y + shakeY,
		});
		sprite->SetSize({
			layout.size.x * popScale * kStartRunSizeScale,
			layout.size.y * popScale * kStartRunSizeScale,
		});
		sprite->SetRotation({
			0.0f,
			0.0f,
			layout.rotation + 0.012f * std::sin(effectTime * 7.0f + static_cast<float>(i)),
		});
		sprite->SetColor({ 1.0f, 1.0f, 1.0f, appearAlpha * fadeAlpha });
		sprite->Update();
	}

	if (gameplayStarted_ && fadeAlpha <= 0.0f) {
		startRunEffectActive_ = false;
	}
}

void GamePlayScene::InitializeSceneReveal() {
	// 画面を左右半分ずつ覆う黒いスプライトを作る。
	revealLeftSprite_ = std::make_unique<Sprite>();
	revealLeftSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	revealLeftSprite_->SetPosition({ 320.0f, 360.0f });
	revealLeftSprite_->SetSize({ 640.0f, 720.0f });
	revealLeftSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	revealRightSprite_ = std::make_unique<Sprite>();
	revealRightSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	revealRightSprite_->SetPosition({ 960.0f, 360.0f });
	revealRightSprite_->SetSize({ 640.0f, 720.0f });
	revealRightSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	sceneRevealTime_ = 0.0f;
	sceneRevealActive_ = true;
	revealLeftSprite_->Update();
	revealRightSprite_->Update();
}

void GamePlayScene::UpdateSceneReveal() {
	if (!sceneRevealActive_) {
		return;
	}

	// 黒いスプライトを中央から左右へ動かしてゲーム画面を見せる。
	constexpr float kRevealDuration = 0.85f;
	sceneRevealTime_ += kDeltaTime;
	const float t = std::clamp(sceneRevealTime_ / kRevealDuration, 0.0f, 1.0f);
	const float eased = t * t * (3.0f - 2.0f * t);

	revealLeftSprite_->SetPosition({ 320.0f - 650.0f * eased, 360.0f });
	revealRightSprite_->SetPosition({ 960.0f + 650.0f * eased, 360.0f });
	revealLeftSprite_->Update();
	revealRightSprite_->Update();

	if (t >= 1.0f) {
		sceneRevealActive_ = false;
	}
}

void GamePlayScene::InitializeGameOverEffect() {
	// 白画像を着色し、暗い赤のフラッシュと最後の暗転に使う。
	gameOverTintSprite_ = std::make_unique<Sprite>();
	gameOverTintSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	gameOverTintSprite_->SetPosition({ 640.0f, 360.0f });
	gameOverTintSprite_->SetSize({ 1280.0f, 720.0f });
	gameOverTintSprite_->SetColor({ 0.32f, 0.0f, 0.0f, 0.0f });
	gameOverTintSprite_->Update();

	gameOverFadeSprite_ = std::make_unique<Sprite>();
	gameOverFadeSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	gameOverFadeSprite_->SetPosition({ 640.0f, 360.0f });
	gameOverFadeSprite_->SetSize({ 1280.0f, 720.0f });
	gameOverFadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	gameOverFadeSprite_->Update();

	deathHandStamps_.clear();
	deathHandStamps_.reserve(kDeathHandCount);
	constexpr float kCellWidth = 1280.0f / static_cast<float>(kDeathHandColumns);
	constexpr float kCellHeight = 720.0f / static_cast<float>(kDeathHandRows);
	for (int i = 0; i < kDeathHandCount; ++i) {
		DeathHandStamp stamp{};
		stamp.sprite = std::make_unique<Sprite>();
		stamp.sprite->Initialize(
			SpriteCommon::GetInstance(),
			NextDeathHandRandom01() < 0.5f ? "resources/scene/handL.png" : "resources/scene/handR.png");

		const int column = i % kDeathHandColumns;
		const int row = i / kDeathHandColumns;
		const float jitterX = (NextDeathHandRandom01() - 0.5f) * kCellWidth * 0.90f;
		const float jitterY = (NextDeathHandRandom01() - 0.5f) * kCellHeight * 0.80f;
		const float size = 300.0f + NextDeathHandRandom01() * 200.0f;
		stamp.baseSize = { size, size };
		stamp.appearTime = NextDeathHandRandom01() * 0.62f;
		stamp.sprite->SetPosition({
			(static_cast<float>(column) + 0.5f) * kCellWidth + jitterX,
			(static_cast<float>(row) + 0.5f) * kCellHeight + jitterY,
		});
		stamp.sprite->SetRotation({
			0.0f,
			0.0f,
			-0.75f + NextDeathHandRandom01() * 1.50f,
		});
		stamp.sprite->SetSize({ size * 0.65f, size * 0.65f });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
		stamp.sprite->Update();
		deathHandStamps_.push_back(std::move(stamp));
	}

	gameOverPhase_ = GameOverPhase::None;
	gameOverPhaseTime_ = 0.0f;
}

void GamePlayScene::StartGameOverSequence() {
	// 即座にシーンを変えず、最後の被弾を見せてから暗転する。
	gameOverPhase_ = GameOverPhase::Impact;
	gameOverPhaseTime_ = 0.0f;
	deathAnimationTime_ = 0.0f;
	player_->BeginDeathAnimation();
	if (animatedModel_ && deathAnimation_.duration > 0.0f) {
		animatedModel_->UpdateSkeleton(deathAnimation_, 0.0f);
	}
	startRunEffectActive_ = false;
	sceneRevealActive_ = false;
}

void GamePlayScene::UpdateGameOverSequence() {
	gameOverPhaseTime_ += kDeltaTime;
	float redAlpha = 0.0f;
	float blackAlpha = 0.0f;

	switch (gameOverPhase_) {
	case GameOverPhase::Impact: {
		const float t = std::clamp(gameOverPhaseTime_ / kDeathImpactDuration, 0.0f, 1.0f);
		const float shake = (1.0f - t) * 0.16f;
		camera_->SetTranslate({
			kGameplayCameraPosition.x + std::sin(gameOverPhaseTime_ * 85.0f) * shake,
			kGameplayCameraPosition.y + std::cos(gameOverPhaseTime_ * 73.0f) * shake,
			kGameplayCameraPosition.z,
		});
		redAlpha = (1.0f - t) * 0.60f;
		player_->UpdateTransformOnly();
		if (t >= 1.0f) {
			gameOverPhase_ = GameOverPhase::Falling;
			gameOverPhaseTime_ = 0.0f;
			camera_->SetTranslate(kGameplayCameraPosition);
		}
		break;
	}
	case GameOverPhase::Falling: {
		const float duration = (std::max)(deathAnimation_.duration, kDeltaTime);
		deathAnimationTime_ = (std::min)(gameOverPhaseTime_, duration);
		const float t = std::clamp(deathAnimationTime_ / duration, 0.0f, 1.0f);
		if (animatedModel_) {
			animatedModel_->UpdateSkeleton(deathAnimation_, deathAnimationTime_);
		}
		player_->UpdateTransformOnly();
		redAlpha = 0.10f * (1.0f - t);
		if (t >= 1.0f) {
			gameOverPhase_ = GameOverPhase::Hold;
			gameOverPhaseTime_ = 0.0f;
		}
		break;
	}
	case GameOverPhase::Hold:
		if (animatedModel_) {
			animatedModel_->UpdateSkeleton(deathAnimation_, deathAnimation_.duration);
		}
		player_->UpdateTransformOnly();
		if (gameOverPhaseTime_ >= kDeathHoldDuration) {
			gameOverPhase_ = GameOverPhase::HandCover;
			gameOverPhaseTime_ = 0.0f;
		}
		break;
	case GameOverPhase::HandCover: {
		const float t = std::clamp(gameOverPhaseTime_ / kDeathHandCoverDuration, 0.0f, 1.0f);
		if (animatedModel_) {
			animatedModel_->UpdateSkeleton(deathAnimation_, deathAnimation_.duration);
		}
		player_->UpdateTransformOnly();
		redAlpha = SmoothStep(t) * 0.58f;
		UpdateDeathHands(gameOverPhaseTime_);
		if (t >= 1.0f) {
			gameOverPhase_ = GameOverPhase::FadeOut;
			gameOverPhaseTime_ = 0.0f;
		}
		break;
	}
	case GameOverPhase::FadeOut: {
		const float t = std::clamp(gameOverPhaseTime_ / kDeathFadeDuration, 0.0f, 1.0f);
		if (animatedModel_) {
			animatedModel_->UpdateSkeleton(deathAnimation_, deathAnimation_.duration);
		}
		player_->UpdateTransformOnly();
		redAlpha = 0.58f;
		blackAlpha = SmoothStep(t);
		UpdateDeathHands(kDeathHandCoverDuration);
		if (t >= 1.0f) {
			sceneManager_->SetNextScene(std::make_unique<GameOverScene>());
		}
		break;
	}
	case GameOverPhase::None:
		break;
	}

	camera_->Update();
	gameOverTintSprite_->SetColor({ 0.32f, 0.0f, 0.0f, redAlpha });
	gameOverTintSprite_->Update();
	gameOverFadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, blackAlpha });
	gameOverFadeSprite_->Update();
}

void GamePlayScene::UpdateDeathHands(float time) {
	constexpr float kStampPopDuration = 0.14f;
	for (DeathHandStamp& stamp : deathHandStamps_) {
		const float t = std::clamp(
			(time - stamp.appearTime) / kStampPopDuration, 0.0f, 1.0f);
		const float eased = SmoothStep(t);
		const float scale = 0.65f + eased * 0.35f;
		stamp.sprite->SetSize({ stamp.baseSize.x * scale, stamp.baseSize.y * scale });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, eased });
		stamp.sprite->Update();
	}
}

float GamePlayScene::NextDeathHandRandom01() {
	deathHandRandomState_ = deathHandRandomState_ * 1664525u + 1013904223u;
	return static_cast<float>((deathHandRandomState_ >> 8) & 0x00FFFFFFu) / 16777215.0f;
}

void GamePlayScene::Draw() {
	// 3Dゲーム画面を描いた後にHPと遷移用スプライトを重ねる。
	courseManager_->Draw();
	if (gameplayStarted_) {
		obstacleManager_->Draw();
	}
	if (ghostObject_) {
		ghostObject_->Draw();
	}
	player_->Draw();

	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	if (gameplayStarted_) {
		for (int i = 0; i < player_->GetHp(); ++i) {
			hpSprites_[i]->Draw(commandList);
		}
	}
	if (startRunEffectActive_) {
		for (const auto& sprite : startRunSprites_) {
			sprite->Draw(commandList);
		}
	}
	if (sceneRevealActive_) {
		revealLeftSprite_->Draw(commandList);
		revealRightSprite_->Draw(commandList);
	}
	if (gameOverPhase_ != GameOverPhase::None) {
		gameOverTintSprite_->Draw(commandList);
		for (const DeathHandStamp& stamp : deathHandStamps_) {
			stamp.sprite->Draw(commandList);
		}
		gameOverFadeSprite_->Draw(commandList);
	}
}
