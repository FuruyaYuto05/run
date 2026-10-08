#pragma once
#include "BaseScene.h"
#include <cstdint>
#include <memory>

class Sprite;
class Camera;
class TitleRunner;
class TitleSceneState;
class TitleIntroState;
class TitleIdleState;
class TitleRestartFadeState;
class TitleExitState;

class TitleScene : public BaseScene {
public:
	TitleScene();
	~TitleScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
private:
	friend class TitleIntroState;
	friend class TitleIdleState;
	friend class TitleRestartFadeState;
	friend class TitleExitState;

	enum class MenuItem {
		Play,
		Exit,
	};

	void ApplyLogoTransform(float scale, float yOffset, float rotationZ, float alpha, float rotationX = 0.0f);
	void ApplyMenuTransform();
	void RequestStateChange(std::unique_ptr<TitleSceneState> state);
	void ApplyPendingState();
	void UpdateOrbitCamera();
	void SelectNextOrbitTarget();
	float NextRandom01();

	std::unique_ptr<Sprite> titleSprite_;
	std::unique_ptr<Sprite> shadowSprite_;
	std::unique_ptr<Sprite> playGlowSprite_;
	std::unique_ptr<Sprite> exitGlowSprite_;
	std::unique_ptr<Sprite> fadeSprite_;
	std::unique_ptr<Sprite> playSprite_;
	std::unique_ptr<Sprite> exitSprite_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<TitleRunner> runner_;
	std::unique_ptr<TitleSceneState> currentState_;
	std::unique_ptr<TitleSceneState> nextState_;
	MenuItem selectedMenu_ = MenuItem::Play;
	bool menuReady_ = false;
	float phaseTime_ = 0.0f;
	float totalTime_ = 0.0f;
	float inactivityTime_ = 0.0f;
	float fadeAlpha_ = 0.0f;
	float orbitAngle_ = 0.0f;
	float orbitSpeed_ = 0.42f;
	float orbitTargetSpeed_ = 0.42f;
	float orbitRadius_ = 14.0f;
	float orbitTargetRadius_ = 14.0f;
	float orbitHeight_ = 3.0f;
	float orbitTargetHeight_ = 3.0f;
	float orbitChangeTimer_ = 0.0f;
	uint32_t orbitRandomState_ = 0x6D2B79F5u;
	bool restartFadingIn_ = false;
	bool previousEnterPressed_ = false;
	bool previousUpPressed_ = false;
	bool previousDownPressed_ = false;
	bool enterTriggered_ = false;
	bool upTriggered_ = false;
	bool downTriggered_ = false;
};
