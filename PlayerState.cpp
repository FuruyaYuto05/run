#include "PlayerState.h"
#include "Player.h"
#include <memory>

namespace {
constexpr int kInvincibleFrames = 120;
constexpr int kBlinkIntervalFrames = 6;
constexpr int kDeadWaitFrames = 45;
}

void PlayerNormalState::Enter(Player& player) {
	player.invincibleTimer_ = 0;
	player.isVisible_ = true;
}

void PlayerNormalState::Update(Player& player) {
	player.UpdateLaneMovement();
}

void PlayerNormalState::OnCollision(Player& player) {
	player.Damage();
	if (player.hp_ <= 0) {
		player.RequestStateChange(std::make_unique<PlayerDeadState>());
	} else {
		player.RequestStateChange(std::make_unique<PlayerInvincibleState>());
	}
}

void PlayerInvincibleState::Enter(Player& player) {
	player.invincibleTimer_ = kInvincibleFrames;
	player.isVisible_ = false;
}

void PlayerInvincibleState::Update(Player& player) {
	if (player.invincibleTimer_ > 0) {
		--player.invincibleTimer_;
		player.isVisible_ = ((player.invincibleTimer_ / kBlinkIntervalFrames) % 2) == 0;
	}

	player.UpdateLaneMovement();

	if (player.invincibleTimer_ <= 0) {
		player.RequestStateChange(std::make_unique<PlayerNormalState>());
	}
}

void PlayerInvincibleState::OnCollision(Player&) {
	// 無敵状態は衝突を受け付けない。
}

void PlayerDeadState::Enter(Player& player) {
	player.deathTimer_ = kDeadWaitFrames;
	player.isVisible_ = true;
	player.gameOverRequested_ = false;
}

void PlayerDeadState::Update(Player& player) {
	if (player.deathTimer_ > 0) {
		--player.deathTimer_;
	}
	if (player.deathTimer_ <= 0) {
		player.gameOverRequested_ = true;
	}
}

void PlayerDeadState::OnCollision(Player&) {
	// 死亡後は追加のダメージを受け付けない。
}
