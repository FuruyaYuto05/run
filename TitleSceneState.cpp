#include "TitleSceneState.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"
#include "TitleRunner.h"
#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kIntroDuration = 1.0f;
constexpr float kExitDuration = 0.45f;
constexpr float kLogoSpinDelay = 2.5f;
constexpr float kLogoSpinInterval = 4.0f;
constexpr float kLogoSpinDuration = 0.45f;
constexpr float kRestartWaitDuration = 8.0f;
constexpr float kRestartFadeDuration = 0.75f;
constexpr float kTwoPi = 6.2831853f;

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

void TitleIntroState::Enter(TitleScene& scene) {
	scene.phaseTime_ = 0.0f;
}

void TitleIntroState::Update(TitleScene& scene) {
	const float t = std::clamp(scene.phaseTime_ / kIntroDuration, 0.0f, 1.0f);
	const float eased = EaseOutBack(t);
	const float scale = 0.1f + 0.9f * eased;
	const float yOffset = 80.0f * (1.0f - eased);
	const float rotation = -0.12f * (1.0f - t);
	scene.ApplyLogoTransform(scale, yOffset, rotation, SmoothStep(t));

	if (t >= 1.0f) {
		scene.RequestStateChange(std::make_unique<TitleIdleState>());
	}
}

void TitleIdleState::Enter(TitleScene& scene) {
	scene.phaseTime_ = 0.0f;
}

void TitleIdleState::Update(TitleScene& scene) {
	scene.inactivityTime_ += kDeltaTime;
	const float wave = std::sin(scene.totalTime_ * 2.8f);

	float spinProgress = 0.0f;
	if (scene.phaseTime_ >= kLogoSpinDelay) {
		const float spinCycle = std::fmod(scene.phaseTime_ - kLogoSpinDelay, kLogoSpinInterval);
		if (spinCycle < kLogoSpinDuration) {
			spinProgress = SmoothStep(spinCycle / kLogoSpinDuration);
		}
	}

	const float spinPulse = std::sin(spinProgress * 3.1415927f);
	const float scale = 1.0f + wave * 0.012f + spinPulse * 0.04f;
	const float yOffset = wave * 6.0f;
	const float wobbleRotation = std::sin(scene.totalTime_ * 1.7f) * 0.012f;
	const float spinRotationX = spinProgress * kTwoPi;
	scene.ApplyLogoTransform(scale, yOffset, wobbleRotation, 1.0f, spinRotationX);

	if (scene.inactivityTime_ >= kRestartWaitDuration) {
		scene.RequestStateChange(std::make_unique<TitleRestartFadeState>());
		return;
	}

	if (!scene.menuReady_) {
		return;
	}

	if (scene.upTriggered_ || scene.downTriggered_) {
		scene.inactivityTime_ = 0.0f;
		scene.selectedMenu_ = scene.selectedMenu_ == TitleScene::MenuItem::Play
			? TitleScene::MenuItem::Exit
			: TitleScene::MenuItem::Play;
	}

	if (scene.enterTriggered_) {
		if (scene.selectedMenu_ == TitleScene::MenuItem::Play) {
			scene.RequestStateChange(std::make_unique<TitleExitState>());
		} else {
			PostQuitMessage(0);
		}
	}
}

void TitleRestartFadeState::Enter(TitleScene& scene) {
	scene.phaseTime_ = 0.0f;
}

void TitleRestartFadeState::Update(TitleScene& scene) {
	const float wave = std::sin(scene.totalTime_ * 2.8f);
	scene.ApplyLogoTransform(
		1.0f + wave * 0.012f,
		wave * 6.0f,
		std::sin(scene.totalTime_ * 1.7f) * 0.012f,
		1.0f);

	const float t = std::clamp(scene.phaseTime_ / kRestartFadeDuration, 0.0f, 1.0f);
	scene.fadeAlpha_ = SmoothStep(t);
	if (t < 1.0f) {
		return;
	}

	scene.totalTime_ = 0.0f;
	scene.inactivityTime_ = 0.0f;
	scene.menuReady_ = false;
	scene.selectedMenu_ = TitleScene::MenuItem::Play;
	scene.fadeAlpha_ = 1.0f;
	scene.restartFadingIn_ = true;
	scene.runner_->ResetRun();
	scene.ApplyLogoTransform(0.1f, 80.0f, -0.12f, 0.0f);
	scene.RequestStateChange(std::make_unique<TitleIntroState>());
}

void TitleExitState::Enter(TitleScene& scene) {
	scene.phaseTime_ = 0.0f;
}

void TitleExitState::Update(TitleScene& scene) {
	const float t = std::clamp(scene.phaseTime_ / kExitDuration, 0.0f, 1.0f);
	const float eased = SmoothStep(t);
	scene.ApplyLogoTransform(1.0f + eased * 0.15f, -80.0f * eased, 0.0f, 1.0f - eased);
	if (t >= 1.0f) {
		scene.sceneManager_->SetNextScene(std::make_unique<GamePlayScene>());
	}
}
