#pragma once
#include "Framework.h"
class SceneManager;
class Game : public Framework {
public:
	void Initialize() override; void Finalize() override; void Update() override; void Draw() override;
private:
	SceneManager* sceneManager_ = nullptr;
};
