#include "TitleScene.h"
#include "Camera.h"
#include "GamePlayScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "DirectXCommon.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "TitleRunner.h"
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
constexpr float kLogoX = 670.0f;
constexpr float kLogoY = 245.0f;
constexpr float kLogoWidth = 704.0f;
constexpr float kLogoHeight = 120.0f;
constexpr float kMenuX = 640.0f;
constexpr float kPlayY = 430.0f;
constexpr float kExitY = 525.0f;
constexpr float kMenuWidth = 320.0f;
constexpr float kMenuHeight = 96.0f;
constexpr float kPlaySlideStartX = 1500.0f;
constexpr float kExitSlideStartX = -220.0f;
constexpr float kPlaySlideDelay = 0.50f;
constexpr float kExitSlideDelay = 0.62f;
constexpr float kMenuSlideDuration = 0.72f;
constexpr float kLogoSpinDelay = 2.5f;
constexpr float kLogoSpinInterval = 4.0f;
constexpr float kLogoSpinDuration = 0.45f;
constexpr float kRestartWaitDuration = 8.0f;
constexpr float kRestartFadeDuration = 0.75f;
constexpr float kHandCoverDuration = 1.15f;
constexpr float kHandFadeStart = 0.72f;
constexpr float kHandFadeDuration = 0.30f;
constexpr float kTwoPi = 6.2831853f;
constexpr float kRunnerFocusHeight = 2.5f;
constexpr int kHandColumns = 6;
constexpr int kHandRows = 4;
constexpr int kHandStampCount = kHandColumns * kHandRows;

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
	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.15f, 0.0f, 0.0f });
	camera_->SetTranslate({ 0.0f, 2.5f, -12.0f });
	object3dCommon->SetDefaultCamera(camera_.get());
	camera_->Update();

	runner_ = std::make_unique<TitleRunner>();
	runner_->Initialize(object3dCommon, camera_.get());
	orbitAngle_ = 0.0f;
	orbitSpeed_ = 0.42f;
	orbitTargetSpeed_ = orbitSpeed_;
	orbitRadius_ = 14.0f;
	orbitTargetRadius_ = orbitRadius_;
	orbitHeight_ = 3.0f;
	orbitTargetHeight_ = orbitHeight_;
	orbitChangeTimer_ = 0.0f;
	UpdateOrbitCamera();

	TextureManager::GetInstance()->LoadTexture("resources/title/title.png");
	TextureManager::GetInstance()->LoadTexture("resources/title/PLAY.png");
	TextureManager::GetInstance()->LoadTexture("resources/title/EXIT.png");
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handL.png");
	TextureManager::GetInstance()->LoadTexture("resources/scene/handR.png");

	shadowSprite_ = std::make_unique<Sprite>();
	shadowSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/title.png");

	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/title.png");

	playSprite_ = std::make_unique<Sprite>();
	playSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/PLAY.png");

	exitSprite_ = std::make_unique<Sprite>();
	exitSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/EXIT.png");

	playGlowSprite_ = std::make_unique<Sprite>();
	playGlowSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/PLAY.png");

	exitGlowSprite_ = std::make_unique<Sprite>();
	exitGlowSprite_->Initialize(SpriteCommon::GetInstance(), "resources/title/EXIT.png");

	// 完全不透明な白画像を黒く着色し、画面全体の暗転に使用する。
	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	fadeSprite_->SetPosition({ 640.0f, 360.0f });
	fadeSprite_->SetSize({ 1280.0f, 720.0f });
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	InitializeHandTransition();

	phase_ = Phase::Intro;
	selectedMenu_ = MenuItem::Play;
	menuReady_ = false;
	phaseTime_ = 0.0f;
	totalTime_ = 0.0f;
	inactivityTime_ = 0.0f;
	fadeAlpha_ = 0.0f;
	restartFadingIn_ = false;
	previousEnterPressed_ = input_->Pushkey(DIK_RETURN);
	previousUpPressed_ = input_->Pushkey(DIK_UP) || input_->Pushkey(DIK_W);
	previousDownPressed_ = input_->Pushkey(DIK_DOWN) || input_->Pushkey(DIK_S);
	ApplyLogoTransform(0.1f, 80.0f, -0.12f, 0.0f);
	ApplyMenuTransform();
}

void TitleScene::Finalize() {
	runner_->Finalize();
	runner_.reset();
	camera_.reset();
	handStamps_.clear();
	playGlowSprite_.reset();
	exitGlowSprite_.reset();
	fadeSprite_.reset();
	playSprite_.reset();
	exitSprite_.reset();
	titleSprite_.reset();
	shadowSprite_.reset();
}

void TitleScene::Update() {
	phaseTime_ += kDeltaTime;
	totalTime_ += kDeltaTime;

	// 再スタート時は、タイトルの飛び出し演出を見せながら黒画面を戻す。
	if (restartFadingIn_) {
		fadeAlpha_ = (std::max)(0.0f, fadeAlpha_ - kDeltaTime / kRestartFadeDuration);
		if (fadeAlpha_ <= 0.0f) {
			restartFadingIn_ = false;
		}
	}

	const bool enterPressed = input_->Pushkey(DIK_RETURN);
	const bool enterTriggered = enterPressed && !previousEnterPressed_;
	previousEnterPressed_ = enterPressed;
	const bool upPressed = input_->Pushkey(DIK_UP) || input_->Pushkey(DIK_W);
	const bool downPressed = input_->Pushkey(DIK_DOWN) || input_->Pushkey(DIK_S);
	const bool upTriggered = upPressed && !previousUpPressed_;
	const bool downTriggered = downPressed && !previousDownPressed_;
	previousUpPressed_ = upPressed;
	previousDownPressed_ = downPressed;
	menuReady_ = totalTime_ >= kExitSlideDelay + kMenuSlideDuration;

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
		inactivityTime_ += kDeltaTime;
		const float wave = std::sin(totalTime_ * 2.8f);

		// 普段は小さく揺れ、一定間隔で一度だけ素早く回転する。
		float spinProgress = 0.0f;
		if (phaseTime_ >= kLogoSpinDelay) {
			const float spinCycle = std::fmod(phaseTime_ - kLogoSpinDelay, kLogoSpinInterval);
			if (spinCycle < kLogoSpinDuration) {
				spinProgress = SmoothStep(spinCycle / kLogoSpinDuration);
			}
		}

		const float spinPulse = std::sin(spinProgress * 3.1415927f);
		const float scale = 1.0f + wave * 0.012f + spinPulse * 0.04f;
		const float yOffset = wave * 6.0f;
		const float wobbleRotation = std::sin(totalTime_ * 1.7f) * 0.012f;
		const float spinRotationX = spinProgress * kTwoPi;
		ApplyLogoTransform(scale, yOffset, wobbleRotation, 1.0f, spinRotationX);
		if (inactivityTime_ >= kRestartWaitDuration) {
			phase_ = Phase::RestartFadeOut;
			phaseTime_ = 0.0f;
		}
		break;
	}
	case Phase::RestartFadeOut: {
		const float wave = std::sin(totalTime_ * 2.8f);
		ApplyLogoTransform(
			1.0f + wave * 0.012f,
			wave * 6.0f,
			std::sin(totalTime_ * 1.7f) * 0.012f,
			1.0f
		);
		const float t = std::clamp(phaseTime_ / kRestartFadeDuration, 0.0f, 1.0f);
		fadeAlpha_ = SmoothStep(t);
		if (t >= 1.0f) {
			phase_ = Phase::Intro;
			phaseTime_ = 0.0f;
			totalTime_ = 0.0f;
			inactivityTime_ = 0.0f;
			menuReady_ = false;
			selectedMenu_ = MenuItem::Play;
			fadeAlpha_ = 1.0f;
			restartFadingIn_ = true;
			runner_->ResetRun();
			ApplyLogoTransform(0.1f, 80.0f, -0.12f, 0.0f);
		}
		break;
	}
	case Phase::Exit: {
		const float logoT = std::clamp(phaseTime_ / kExitDuration, 0.0f, 1.0f);
		const float eased = SmoothStep(logoT);
		ApplyLogoTransform(1.0f + eased * 0.15f, -80.0f * eased, 0.0f, 1.0f - eased);
		UpdateHandTransition();
		const float blackT = std::clamp(
			(phaseTime_ - kHandFadeStart) / kHandFadeDuration, 0.0f, 1.0f);
		fadeAlpha_ = SmoothStep(blackT);
		if (phaseTime_ >= kHandCoverDuration) {
			sceneManager_->SetNextScene(std::make_unique<GamePlayScene>());
		}
		break;
	}
	}

	if (phase_ == Phase::Idle && menuReady_) {
		if (upTriggered || downTriggered) {
			inactivityTime_ = 0.0f;
			selectedMenu_ = selectedMenu_ == MenuItem::Play ? MenuItem::Exit : MenuItem::Play;
		}

		if (enterTriggered) {
			if (selectedMenu_ == MenuItem::Play) {
				phase_ = Phase::Exit;
				phaseTime_ = 0.0f;
				fadeAlpha_ = 0.0f;
				ResetHandTransition();
			} else {
				PostQuitMessage(0);
			}
		}
	}

	runner_->Update();
	UpdateOrbitCamera();
	camera_->Update();
	ApplyMenuTransform();

	shadowSprite_->Update();
	titleSprite_->Update();
	playGlowSprite_->Update();
	exitGlowSprite_->Update();
	playSprite_->Update();
	exitSprite_->Update();
	fadeSprite_->SetColor({ 0.0f, 0.0f, 0.0f, fadeAlpha_ });
	fadeSprite_->Update();

#ifdef USE_IMGUI
	ImGui::Begin("Title Scene");
	ImGui::Text("Press Enter to Start");
	ImGui::Text("Animation Time: %.2f", totalTime_);
	ImGui::Text("Selected: %s", selectedMenu_ == MenuItem::Play ? "PLAY" : "EXIT");
	ImGui::Text("Menu Input: %s", menuReady_ ? "Ready" : "Waiting for slide-in");
	ImGui::Text("Auto Restart: %.1f / %.1f", inactivityTime_, kRestartWaitDuration);
	ImGui::End();
	runner_->DrawImGui();
#endif
}

void TitleScene::InitializeHandTransition() {
	handStamps_.clear();
	handStamps_.reserve(kHandStampCount);
	for (int i = 0; i < kHandStampCount; ++i) {
		HandStamp stamp{};
		stamp.sprite = std::make_unique<Sprite>();
		stamp.sprite->Initialize(
			SpriteCommon::GetInstance(),
			NextHandRandom01() < 0.5f ? "resources/scene/handL.png" : "resources/scene/handR.png");
		handStamps_.push_back(std::move(stamp));
	}
	ResetHandTransition();
}

void TitleScene::ResetHandTransition() {
	constexpr float kCellWidth = 1280.0f / static_cast<float>(kHandColumns);
	constexpr float kCellHeight = 720.0f / static_cast<float>(kHandRows);

	for (size_t i = 0; i < handStamps_.size(); ++i) {
		HandStamp& stamp = handStamps_[i];
		const int column = static_cast<int>(i) % kHandColumns;
		const int row = static_cast<int>(i) / kHandColumns;
		const float jitterX = (NextHandRandom01() - 0.5f) * kCellWidth * 0.90f;
		const float jitterY = (NextHandRandom01() - 0.5f) * kCellHeight * 0.80f;
		const float x = (static_cast<float>(column) + 0.5f) * kCellWidth + jitterX;
		const float y = (static_cast<float>(row) + 0.5f) * kCellHeight + jitterY;
		const float size = 300.0f + NextHandRandom01() * 200.0f;
		const float rotation = -0.75f + NextHandRandom01() * 1.50f;

		stamp.baseSize = { size, size };
		stamp.appearTime = NextHandRandom01() * 0.68f;
		stamp.sprite->SetPosition({ x, y });
		stamp.sprite->SetRotation({ 0.0f, 0.0f, rotation });
		stamp.sprite->SetSize({ stamp.baseSize.x * 0.65f, stamp.baseSize.y * 0.65f });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
		stamp.sprite->Update();
	}
}

void TitleScene::UpdateHandTransition() {
	constexpr float kStampPopDuration = 0.14f;
	for (HandStamp& stamp : handStamps_) {
		const float t = std::clamp(
			(phaseTime_ - stamp.appearTime) / kStampPopDuration, 0.0f, 1.0f);
		const float scale = 0.65f + EaseOutBack(t) * 0.35f;
		stamp.sprite->SetSize({ stamp.baseSize.x * scale, stamp.baseSize.y * scale });
		stamp.sprite->SetColor({ 1.0f, 1.0f, 1.0f, SmoothStep(t) });
		stamp.sprite->Update();
	}
}

void TitleScene::UpdateOrbitCamera() {
	if (!camera_ || !runner_) {
		return;
	}

	orbitChangeTimer_ -= kDeltaTime;
	if (orbitChangeTimer_ <= 0.0f) {
		SelectNextOrbitTarget();
	}

	// 速度・距離・高さを急に切り替えず、次のランダム値へゆっくり近づける。
	const float blend = 1.0f - std::exp(-kDeltaTime * 0.80f);
	orbitSpeed_ += (orbitTargetSpeed_ - orbitSpeed_) * blend;
	orbitRadius_ += (orbitTargetRadius_ - orbitRadius_) * blend;
	orbitHeight_ += (orbitTargetHeight_ - orbitHeight_) * blend;
	orbitAngle_ = std::fmod(orbitAngle_ + orbitSpeed_ * kDeltaTime, kTwoPi);

	const Math::Vector3 runnerPosition = runner_->GetPosition();
	const Math::Vector3 focus = {
		runnerPosition.x,
		runnerPosition.y + kRunnerFocusHeight,
		runnerPosition.z
	};
	const Math::Vector3 cameraPosition = {
		focus.x + std::sin(orbitAngle_) * orbitRadius_,
		focus.y + orbitHeight_,
		focus.z - std::cos(orbitAngle_) * orbitRadius_
	};

	const float horizontalDistance = std::sqrt(
		(focus.x - cameraPosition.x) * (focus.x - cameraPosition.x) +
		(focus.z - cameraPosition.z) * (focus.z - cameraPosition.z));
	const float pitch = std::atan2(cameraPosition.y - focus.y, horizontalDistance);
	const float yaw = std::atan2(focus.x - cameraPosition.x, focus.z - cameraPosition.z);

	camera_->SetTranslate(cameraPosition);
	camera_->SetRotate({ pitch, yaw, 0.0f });
}

void TitleScene::SelectNextOrbitTarget() {
	// 横方向には回り続けつつ、特徴の異なる3種類の構図をランダムに選ぶ。
	// 約11～17秒で1周する範囲。タイトル表示中にも周回が目で分かる速さにする。
	orbitTargetSpeed_ = 0.38f + NextRandom01() * 0.17f;

	const float shotType = NextRandom01();
	if (shotType < 0.34f) {
		// 近距離：キャラクターを大きく映す。
		orbitTargetRadius_ = 6.5f + NextRandom01() * 2.5f;
		orbitTargetHeight_ = 1.8f + NextRandom01() * 2.5f;
	} else if (shotType < 0.67f) {
		// 俯瞰：上から走っている姿を見下ろす。
		orbitTargetRadius_ = 10.0f + NextRandom01() * 4.0f;
		orbitTargetHeight_ = 8.0f + NextRandom01() * 4.0f;
	} else {
		// 遠距離：キャラクターと周囲を広く映す。
		orbitTargetRadius_ = 18.0f + NextRandom01() * 5.0f;
		orbitTargetHeight_ = 3.5f + NextRandom01() * 4.0f;
	}

	orbitChangeTimer_ = 3.5f + NextRandom01() * 3.0f;
}

float TitleScene::NextRandom01() {
	orbitRandomState_ = orbitRandomState_ * 1664525u + 1013904223u;
	return static_cast<float>((orbitRandomState_ >> 8) & 0x00FFFFFFu) / 16777215.0f;
}

float TitleScene::NextHandRandom01() {
	handRandomState_ = handRandomState_ * 1664525u + 1013904223u;
	return static_cast<float>((handRandomState_ >> 8) & 0x00FFFFFFu) / 16777215.0f;
}

void TitleScene::ApplyMenuTransform() {
	const float playT = std::clamp((totalTime_ - kPlaySlideDelay) / kMenuSlideDuration, 0.0f, 1.0f);
	const float exitT = std::clamp((totalTime_ - kExitSlideDelay) / kMenuSlideDuration, 0.0f, 1.0f);
	const float playX = kPlaySlideStartX + (kMenuX - kPlaySlideStartX) * EaseOutBack(playT);
	const float exitX = kExitSlideStartX + (kMenuX - kExitSlideStartX) * EaseOutBack(exitT);

	float menuAlpha = SmoothStep(exitT);
	if (phase_ == Phase::Exit) {
		menuAlpha *= 1.0f - std::clamp(phaseTime_ / kExitDuration, 0.0f, 1.0f);
	}
	const float pulse = 1.0f + std::sin(totalTime_ * 5.0f) * 0.04f;
	const float playScale = menuReady_ && selectedMenu_ == MenuItem::Play ? pulse : 0.86f;
	const float exitScale = menuReady_ && selectedMenu_ == MenuItem::Exit ? pulse : 0.86f;
	const float glowWave = 0.5f + std::sin(totalTime_ * 6.0f) * 0.5f;
	const float playGlow = menuReady_
		? (selectedMenu_ == MenuItem::Play ? 0.38f + glowWave * 0.28f : 0.12f)
		: 0.0f;
	const float exitGlow = menuReady_
		? (selectedMenu_ == MenuItem::Exit ? 0.38f + glowWave * 0.28f : 0.12f)
		: 0.0f;

	playSprite_->SetPosition({ playX, kPlayY });
	playSprite_->SetSize({ kMenuWidth * playScale, kMenuHeight * playScale });
	playSprite_->SetColor({ 1.0f, 1.0f, 1.0f, SmoothStep(playT) * menuAlpha });

	exitSprite_->SetPosition({ exitX, kExitY });
	exitSprite_->SetSize({ kMenuWidth * exitScale, kMenuHeight * exitScale });
	exitSprite_->SetColor({ 1.0f, 1.0f, 1.0f, SmoothStep(exitT) * menuAlpha });

	playGlowSprite_->SetPosition({ playX, kPlayY });
	playGlowSprite_->SetSize({ kMenuWidth * playScale * 1.12f, kMenuHeight * playScale * 1.18f });
	playGlowSprite_->SetColor({ 1.0f, 0.75f, 0.15f, playGlow * menuAlpha });

	exitGlowSprite_->SetPosition({ exitX, kExitY });
	exitGlowSprite_->SetSize({ kMenuWidth * exitScale * 1.12f, kMenuHeight * exitScale * 1.18f });
	exitGlowSprite_->SetColor({ 0.15f, 1.0f, 0.80f, exitGlow * menuAlpha });
}

void TitleScene::ApplyLogoTransform(float scale, float yOffset, float rotationZ, float alpha, float rotationX) {
	const Math::Vector2 size = { kLogoWidth * scale, kLogoHeight * scale };
	const Math::Vector2 position = { kLogoX, kLogoY + yOffset };

	titleSprite_->SetPosition(position);
	titleSprite_->SetSize(size);
	titleSprite_->SetRotation({ rotationX, 0.0f, rotationZ });
	titleSprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha });

	shadowSprite_->SetPosition({ position.x + 8.0f, position.y + 8.0f });
	shadowSprite_->SetSize(size);
	shadowSprite_->SetRotation({ rotationX, 0.0f, rotationZ });
	shadowSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha * 0.35f });
}

void TitleScene::Draw() {
	runner_->Draw();

	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	shadowSprite_->Draw(commandList);
	titleSprite_->Draw(commandList);
	playGlowSprite_->Draw(commandList);
	exitGlowSprite_->Draw(commandList);
	playSprite_->Draw(commandList);
	exitSprite_->Draw(commandList);
	for (const HandStamp& stamp : handStamps_) {
		stamp.sprite->Draw(commandList);
	}
	fadeSprite_->Draw(commandList);
}
