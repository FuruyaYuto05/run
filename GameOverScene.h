#pragma once

#include "BaseScene.h"
#include "Math.h"
#include <cstdint>
#include <memory>
#include <vector>

class Sprite;
class Camera;
class Object3d;

class GameOverScene : public BaseScene {
public:
	GameOverScene(int distance = 0, int souls = 0, int score = 0);
	~GameOverScene() override;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	enum class MenuItem {
		Retry,
		Title,
	};

	struct HandStamp {
		std::unique_ptr<Sprite> sprite;
		Math::Vector2 baseSize{};
		float appearTime = 0.0f;
	};

	// お札の周囲を漂う、3Dモデルの魂。
	struct FloatingSoul {
		std::unique_ptr<Object3d> object;
		Math::Vector3 basePosition{};
		float phase = 0.0f;
	};

	void InitializeHands();
	void UpdateHands();
	void InitializeFloatingSouls();
	void UpdateFloatingSouls();
	void InitializeResultNumbers();
	void UpdateResultNumbers();
	void UpdateLayout();
	float NextRandom01();

	std::unique_ptr<Sprite> backgroundSprite_;
	std::unique_ptr<Sprite> resultOfudaSprite_;
	std::unique_ptr<Sprite> retrySprite_;
	std::unique_ptr<Sprite> toTitleSprite_;
	std::unique_ptr<Sprite> newRecordSprite_;
	std::unique_ptr<Sprite> achievementToastSprite_;
	std::unique_ptr<Sprite> fadeSprite_;
	std::vector<HandStamp> handStamps_;
	std::unique_ptr<Sprite> resultDistanceLabelSprite_;
	std::unique_ptr<Sprite> resultSoulsLabelSprite_;
	std::unique_ptr<Sprite> resultScoreLabelSprite_;
	std::vector<std::unique_ptr<Sprite>> distanceDigitSprites_;
	std::vector<std::unique_ptr<Sprite>> soulsDigitSprites_;
	std::vector<std::unique_ptr<Sprite>> scoreDigitSprites_;
	std::unique_ptr<Camera> resultCamera_;
	std::vector<FloatingSoul> floatingSouls_;
	int resultDistance_ = 0;
	int resultSouls_ = 0;
	int resultScore_ = 0;
	MenuItem selectedMenu_ = MenuItem::Retry;
	float elapsedTime_ = 0.0f;
	float fadeTime_ = 0.0f;
	bool menuReady_ = false;
	bool fadingOut_ = false;
	bool previousEnterPressed_ = false;
	bool previousUpPressed_ = false;
	bool previousDownPressed_ = false;
	uint32_t randomState_ = 0x91E10DA5u;
};
