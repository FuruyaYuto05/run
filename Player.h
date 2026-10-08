#pragma once

#include "Math.h"
#include <memory>

class Object3d;
class Object3dCommon;
class Input;
class PlayerState;
class PlayerNormalState;
class PlayerInvincibleState;
class PlayerDeadState;

class Player {
public:
	Player();
	~Player();

	void Initialize(Object3dCommon* object3dCommon, Input* input);
	void Finalize();
	void Update();
	void Draw();
	void DrawImGui();
	void OnCollision();
	bool IsInvincible() const;
	int GetHp() const { return hp_; }
	bool IsDead() const;
	bool IsGameOverRequested() const { return gameOverRequested_; }
	const char* GetStateName() const;
	const Math::Vector3& GetPosition() const;

private:
	friend class PlayerNormalState;
	friend class PlayerInvincibleState;
	friend class PlayerDeadState;

	void RequestStateChange(std::unique_ptr<PlayerState> state);
	void ApplyPendingState();
	void UpdateLaneMovement();
	void Damage();

	std::unique_ptr<Object3d> object3d_;
	std::unique_ptr<PlayerState> currentState_;
	std::unique_ptr<PlayerState> nextState_;
	Input* input_ = nullptr;
	float moveSpeed_ = 0.2f;
	int laneIndex_ = 1; // 0: 左、1: 中央、2: 右（移動先のレーン）
	bool previousLeftPressed_ = false;
	bool previousRightPressed_ = false;
	int invincibleTimer_ = 0;
	int deathTimer_ = 0;
	bool isVisible_ = true;
	bool gameOverRequested_ = false;
	int hp_ = 3;
};
