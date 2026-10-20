#include "GamePlayScene.h"
#include "Camera.h"
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
	TextureManager::GetInstance()->LoadTexture("monsterBall.png");
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
	ModelManager::GetInstance()->LoadModel("road.obj");
	ModelManager::GetInstance()->LoadModel("tyoutin.obj");
	animatedModel_ = ModelManager::GetInstance()->FindModel("human/Mannequin_Large.glb");
	// ゲーム内の各管理オブジェクトを生成する。
	player_ = std::make_unique<Player>();
	player_->Initialize(object3dCommon_, input_);
	courseManager_ = std::make_unique<CourseManager>();
	courseManager_->Initialize(object3dCommon_);
	obstacleManager_ = std::make_unique<ObstacleManager>();
	obstacleManager_->Initialize(object3dCommon_);
	animation_ = LoadAnimationFile("resources/human", "Rig_Large_MovementBasic.glb");
	// 演出中に描画できるよう、各3Dオブジェクトの行列だけ先に作る。
	player_->Update();
	courseManager_->UpdateTransformOnly();
	obstacleManager_->Update(kGameplayCameraPosition.z, 0.0f);
	openingCameraTime_ = 0.0f;
	gameplayStarted_ = false;
	InitializeStartRunEffect();
	InitializeSceneReveal();
}

void GamePlayScene::Finalize() {
	// シーンで生成したオブジェクトと音声を解放する。
	player_->Finalize();
	courseManager_->Finalize();
	obstacleManager_->Finalize();
	player_.reset(); courseManager_.reset(); obstacleManager_.reset();
	hpSprites_.clear();
	startRunSprites_.clear();
	revealLeftSprite_.reset();
	revealRightSprite_.reset();
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
	// 走行アニメーションを繰り返し再生する。
	if (animatedModel_ && animation_.duration > 0) { animationTime_ = std::fmod(animationTime_ + kDeltaTime, animation_.duration); animatedModel_->UpdateSkeleton(animation_, animationTime_); }

	// カメラ演出が終わるまではコースと障害物を動かさない。
	if (!gameplayStarted_) {
		UpdateOpeningCamera();
		UpdateStartRunEffect();
		camera_->Update();
		// 自機は移動させず、移動中のカメラだけを描画へ反映する。
		player_->UpdateTransformOnly();
		// 床は流すが、演出中は再配置を止めて画面内で消えるのを防ぐ。
		courseManager_->Update(camera_->GetTranslate().z, false);
		for (const auto& sprite : hpSprites_) { sprite->Update(); }
		UpdateSceneReveal();
		return;
	}

	// カメラ・プレイヤー・コース・障害物を順番に更新する。
	camera_->Update();
	player_->Update();
	courseManager_->Update(camera_->GetTranslate().z);
	obstacleManager_->Update(camera_->GetTranslate().z, courseManager_->GetMoveSpeed());
	// 無敵中でなければ障害物との当たり判定を行う。
	if (!player_->IsInvincible() && obstacleManager_->CheckCollision(player_->GetPosition())) {
		player_->OnCollision();
		if (player_->IsDead()) {
			sceneManager_->SetNextScene(std::make_unique<GameOverScene>());
			return;
		}
	}
	// HP表示とゲーム開始時の画面開放演出を更新する。
	for (const auto& sprite : hpSprites_) { sprite->Update(); }
	UpdateStartRunEffect();
	UpdateSceneReveal();
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
			kStartRunAppearInterval * static_cast<float>(i);
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
			layout.size.x * popScale,
			layout.size.y * popScale,
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

void GamePlayScene::Draw() {
	// 3Dゲーム画面を描いた後にHPと遷移用スプライトを重ねる。
	courseManager_->Draw();
	if (gameplayStarted_) {
		obstacleManager_->Draw();
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
}
