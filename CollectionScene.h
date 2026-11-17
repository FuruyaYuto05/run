#pragma once

#include "BaseScene.h"
#include <memory>

class Sprite;

// 閉じた実績帳を表示するコレクション画面。
class CollectionScene : public BaseScene {
public:
	CollectionScene();
	~CollectionScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<Sprite> backgroundSprite_;
	std::unique_ptr<Sprite> bookSprite_;
	float enterTime_ = 0.0f;
	float pulseTime_ = 0.0f;
};
