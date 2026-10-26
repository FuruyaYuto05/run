#pragma once

#include "BaseScene.h"
#include "Animation.h"
#include <vector>
#include <memory>
#include <cstdint>

class Camera;
class Object3dCommon;
class Object3d;
class Model;
class Sprite;
class Player;
class CourseManager;
class ObstacleManager;

class GamePlayScene : public BaseScene {
public:
	GamePlayScene();
	~GamePlayScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	enum class GameOverPhase {
		None,
		Impact,
		Falling,
		Hold,
		HandCover,
		FadeOut,
	};

	struct DeathHandStamp {
		std::unique_ptr<Sprite> sprite;
		Math::Vector2 baseSize{};
		float appearTime = 0.0f;
	};

	void UpdateOpeningCamera();
	void UpdateGhost();
	void InitializeStartRunEffect();
	void UpdateStartRunEffect();
	void InitializeSceneReveal();
	void UpdateSceneReveal();
	void InitializeGameOverEffect();
	void StartGameOverSequence();
	void UpdateGameOverSequence();
	void UpdateDeathHands(float time);
	float NextDeathHandRandom01();

	Object3dCommon* object3dCommon_ = nullptr;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Object3d> ghostObject_;
	std::unique_ptr<CourseManager> courseManager_;
	std::unique_ptr<ObstacleManager> obstacleManager_;
	Model* animatedModel_ = nullptr;
	Model* ghostModel_ = nullptr;
	std::vector<std::unique_ptr<Sprite>> hpSprites_;
	std::vector<std::unique_ptr<Sprite>> startRunSprites_;
	std::vector<size_t> startRunAppearRanks_;
	std::unique_ptr<Sprite> revealLeftSprite_;
	std::unique_ptr<Sprite> revealRightSprite_;
	std::unique_ptr<Sprite> gameOverTintSprite_;
	std::unique_ptr<Sprite> gameOverFadeSprite_;
	std::vector<DeathHandStamp> deathHandStamps_;
	Animation animation_{};
	Animation deathAnimation_{};
	Animation ghostAnimation_{};
	float animationTime_ = 0.0f;
	float deathAnimationTime_ = 0.0f;
	float ghostAnimationTime_ = 0.0f;
	float openingCameraTime_ = 0.0f;
	float startRunFadeTime_ = 0.0f;
	float sceneRevealTime_ = 0.0f;
	bool gameplayStarted_ = false;
	bool startRunEffectActive_ = true;
	bool sceneRevealActive_ = true;
	GameOverPhase gameOverPhase_ = GameOverPhase::None;
	float gameOverPhaseTime_ = 0.0f;
	uint32_t deathHandRandomState_ = 0x6D2B79F5u;
};
