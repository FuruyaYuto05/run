#pragma once

#include "BaseScene.h"

class GameOverScene : public BaseScene {
public:
	GameOverScene();
	~GameOverScene() override;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	float elapsedTime_ = 0.0f;
};
