#include "SceneManager.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

SceneManager::~SceneManager() {
	Finalize();
}

void SceneManager::Finalize() {
	if (scene_) {
		scene_->Finalize();
		scene_.reset();
	}
	nextScene_.reset();
}

void SceneManager::Update() {
	// 次シーンが予約されていたら、実行シーンを切り替える
	if (nextScene_) {
		if (scene_) {
			scene_->Finalize();
			scene_.reset();
		}

		scene_ = std::move(nextScene_);
		scene_->SetSceneManager(this);
		scene_->SetInput(input_);
		scene_->Initialize();
	}

	// 実行中のシーンを更新する
	if (scene_) {
		scene_->Update();
	}
}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}
}
