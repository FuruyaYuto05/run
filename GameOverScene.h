#pragma once

#include "BaseScene.h"
#include "Math.h"
#include <cstdint>
#include <memory>
#include <vector>

class Sprite;

class GameOverScene : public BaseScene {
public:
	GameOverScene();
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

	void InitializeHands();
	void UpdateHands();
	void UpdateLayout();
	float NextRandom01();

	std::unique_ptr<Sprite> backgroundSprite_;
	std::unique_ptr<Sprite> gameOverTextSprite_;
	std::unique_ptr<Sprite> retrySprite_;
	std::unique_ptr<Sprite> toTitleSprite_;
	std::unique_ptr<Sprite> fadeSprite_;
	std::vector<HandStamp> handStamps_;
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
