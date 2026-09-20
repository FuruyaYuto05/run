#pragma once

#include "BaseScene.h"
#include <memory>

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
	std::unique_ptr<Sprite> backgroundSprite_;
	bool previousEnterPressed_ = false;
};
