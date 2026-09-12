#pragma once

#include <memory>

class Object3d;
class Object3dCommon;
class Input;

class Player {
public:
	Player();
	~Player();

	void Initialize(Object3dCommon* object3dCommon, Input* input);
	void Finalize();
	void Update();
	void Draw();
	void DrawImGui();

private:
	std::unique_ptr<Object3d> object3d_;
	Input* input_ = nullptr;
	float moveSpeed_ = 0.2f;
	int laneIndex_ = 1; // 0: 左、1: 中央、2: 右（移動先のレーン）
	bool previousLeftPressed_ = false;
	bool previousRightPressed_ = false;
};
