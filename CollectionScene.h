#pragma once

#include "BaseScene.h"
#include <memory>

class Sprite;

// 表紙から見開きの実績一覧へ切り替えるコレクション画面。
class CollectionScene : public BaseScene {
public:
	CollectionScene();
	~CollectionScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	enum class BookState {
		Entering,
		Closed,
		Opening,
		Open,
	};

	std::unique_ptr<Sprite> backgroundSprite_;
	std::unique_ptr<Sprite> bookSprite_;
	std::unique_ptr<Sprite> openBookSprite_;
	std::unique_ptr<Sprite> firstPlayAchievementSprite_;
	BookState bookState_ = BookState::Entering;
	float enterTime_ = 0.0f;
	float pulseTime_ = 0.0f;
	float openingTime_ = 0.0f;
	bool previousSpacePressed_ = false;
};
