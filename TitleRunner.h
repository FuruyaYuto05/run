#pragma once

#include "Animation.h"
#include "Math.h"
#include <memory>

class Camera;
class Model;
class Object3d;
class Object3dCommon;

// タイトル画面の奥を自動で走る、演出専用のキャラクター。
// ゲーム本編のPlayerとは分離し、入力・HP・当たり判定を持たせない。
class TitleRunner {
public:
	TitleRunner();
	~TitleRunner();

	void Initialize(Object3dCommon* object3dCommon, Camera* camera);
	void Finalize();
	void Update();
	void ResetRun();
	Math::Vector3 GetPosition() const;
	void Draw();
	void DrawImGui();

private:
	std::unique_ptr<Object3d> object3d_;
	Model* model_ = nullptr;
	Animation animation_{};
	float animationTime_ = 0.0f;
	float moveSpeed_ = 0.045f;
	float startX_ = -8.5f;
	float endX_ = 8.5f;
	bool isMoving_ = true;
};
