#pragma once

#include "Math.h"
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
	void UpdateTransformOnly();
	void BeginDeathAnimation();
	void Draw();
	void DrawImGui();
	void OnCollision();
	void UpdateSpirit(float deltaSeconds);
	void RecoverSpirit(float amount);
	void RecoverHp(int amount = 1);
	bool IsInvincible() const { return invincibleTimer_ > 0; }
	int GetHp() const { return hp_; }
	bool IsDead() const { return hp_ <= 0; }
	float GetSpiritRatio() const;
	const Math::Vector3& GetPosition() const;

private:
	void ReduceSpirit(float amount);

	std::unique_ptr<Object3d> object3d_;
	Input* input_ = nullptr;
	float moveSpeed_ = 0.2f;
	int laneIndex_ = 1; // 0: 左、1: 中央、2: 右（移動先のレーン）
	bool previousLeftPressed_ = false;
	bool previousRightPressed_ = false;
	bool previousJumpPressed_ = false;
	int invincibleTimer_ = 0;
	bool isVisible_ = true;
	int hp_ = 3;
	float spirit_ = 100.0f;
	float verticalVelocity_ = 0.0f;
	bool isGrounded_ = true;
};
