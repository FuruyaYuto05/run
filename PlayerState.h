#pragma once

class Player;

// Playerが持つ状態の共通インターフェース。
// Playerは具体的な状態を判定せず、このインターフェース経由で処理を委譲する。
class PlayerState {
public:
	virtual ~PlayerState() = default;
	virtual void Enter(Player& player) = 0;
	virtual void Update(Player& player) = 0;
	virtual void OnCollision(Player& player) = 0;
	virtual bool IsInvincible() const { return false; }
	virtual bool IsDead() const { return false; }
	virtual const char* GetName() const = 0;
};

class PlayerNormalState final : public PlayerState {
public:
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void OnCollision(Player& player) override;
	const char* GetName() const override { return "Normal"; }
};

class PlayerInvincibleState final : public PlayerState {
public:
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void OnCollision(Player& player) override;
	bool IsInvincible() const override { return true; }
	const char* GetName() const override { return "Invincible"; }
};

class PlayerDeadState final : public PlayerState {
public:
	void Enter(Player& player) override;
	void Update(Player& player) override;
	void OnCollision(Player& player) override;
	bool IsDead() const override { return true; }
	const char* GetName() const override { return "Dead"; }
};
