#pragma once

class SceneManager;
class Input;

class BaseScene {
public:
	virtual ~BaseScene() = default;
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// SceneManager と入力をフレームワーク側から受け取る
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
	virtual void SetInput(Input* input) { input_ = input; }

protected:
	SceneManager* sceneManager_ = nullptr;
	Input* input_ = nullptr;
};
