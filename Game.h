#pragma once
#include "Framework.h"
class SceneManager;
class Game final : public Framework {
protected:
	void OnInitialize() override;
	void OnFinalize() override;
	void OnUpdate() override;
	void OnDraw() override;

private:
	SceneManager* sceneManager_ = nullptr;
};
