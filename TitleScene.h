#pragma once
#include "BaseScene.h"
#include <memory>

class Sprite;

class TitleScene : public BaseScene {
public:
	TitleScene();
	~TitleScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
private:
	std::unique_ptr<Sprite> titleSprite_;
};
