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
	float moveSpeed_ = 0.1f;
	float moveMinX_ = -2.0f;
	float moveMaxX_ = 2.0f;
};
