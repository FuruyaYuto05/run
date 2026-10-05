#pragma once
#include "BaseScene.h"
#include <memory>

class Sprite;
class Camera;
class TitleRunner;

class TitleScene : public BaseScene {
public:
	TitleScene();
	~TitleScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
private:
	enum class Phase {
		Intro,
		Idle,
		Exit,
	};

	void ApplyLogoTransform(float scale, float yOffset, float rotation, float alpha);

	std::unique_ptr<Sprite> titleSprite_;
	std::unique_ptr<Sprite> shadowSprite_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<TitleRunner> runner_;
	Phase phase_ = Phase::Intro;
	float phaseTime_ = 0.0f;
	float totalTime_ = 0.0f;
	bool previousEnterPressed_ = false;
};
