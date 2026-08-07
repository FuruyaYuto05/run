#pragma once

#include "BaseScene.h"
#include <memory>

class Input;

// シーンの所有・管理を担当するクラス
class SceneManager {
public:
	static SceneManager* GetInstance();

	// 次のシーンを予約する
	void SetNextScene(std::unique_ptr<BaseScene> nextScene) { nextScene_ = std::move(nextScene); }
	void SetInput(Input* input) { input_ = input; }
	void Update();
	void Draw();
	void Finalize();

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	// 今のシーン（実行中のシーン）
	std::unique_ptr<BaseScene> scene_;
	// 次のシーン（予約中のシーン）
	std::unique_ptr<BaseScene> nextScene_;
	Input* input_ = nullptr;
};
