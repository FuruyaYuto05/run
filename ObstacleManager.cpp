#include "ObstacleManager.h"
#include "Obstacle.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr int kObstacleCount = 5;
constexpr float kFirstObstacleZ = 20.0f;
constexpr float kMinObstacleSpacing = 12.0f;
constexpr float kMaxObstacleSpacing = 19.0f;
// tyoutin.objは原点から下へ1mあり、丸太は中心原点なので高さを分ける。
constexpr float kLanternY = 0.65f;
constexpr float kLogY = 0.33f;
constexpr float kRecycleOffset = 2.0f;
constexpr float kLanePositions[] = { -2.0f, 0.0f, 2.0f };
constexpr int kLaneCount = 3;
constexpr float kLanternCollisionHalfWidth = 0.8f;
constexpr float kLanternCollisionHalfDepth = 0.8f;
constexpr float kLogCollisionHalfWidth = 3.4f;
constexpr float kLogCollisionHalfDepth = 0.8f;
constexpr float kLogJumpClearHeight = 0.85f;

float GetObstacleY(ObstacleType type) {
	return type == ObstacleType::Log ? kLogY : kLanternY;
}
}

ObstacleManager::ObstacleManager() = default;
ObstacleManager::~ObstacleManager() = default;

void ObstacleManager::Initialize(Object3dCommon* object3dCommon) {
	obstacles_.reserve(kObstacleCount);

	float nextZ = kFirstObstacleZ;
	for (int i = 0; i < kObstacleCount; ++i) {
		const ObstacleType type = RandomType();
		const float x = type == ObstacleType::Log ? 0.0f : kLanePositions[RandomLaneIndex()];

		auto obstacle = std::make_unique<Obstacle>();
		obstacle->Initialize(object3dCommon, { x, GetObstacleY(type), nextZ }, type);
		obstacles_.push_back(std::move(obstacle));
		nextZ += RandomSpacing();
	}
}

void ObstacleManager::Finalize() {
	for (const auto& obstacle : obstacles_) {
		obstacle->Finalize();
	}
	obstacles_.clear();
}

void ObstacleManager::Update(float cameraZ, float moveSpeed) {
	if (obstacles_.empty()) {
		return;
	}

	for (const auto& obstacle : obstacles_) {
		Math::Vector3 position = obstacle->GetPosition();
		position.z -= moveSpeed;
		obstacle->SetPosition(position);
	}

	float furthestZ = obstacles_.front()->GetPosition().z;
	for (const auto& obstacle : obstacles_) {
		furthestZ = std::max(furthestZ, obstacle->GetPosition().z);
	}

	for (const auto& obstacle : obstacles_) {
		Math::Vector3 position = obstacle->GetPosition();
		if (position.z < cameraZ - kRecycleOffset) {
			furthestZ += RandomSpacing();
			const ObstacleType type = RandomType();
			obstacle->SetType(type);
			position.x = type == ObstacleType::Log ? 0.0f : kLanePositions[RandomLaneIndex()];
			position.y = GetObstacleY(type);
			position.z = furthestZ;
			obstacle->SetPosition(position);
		}
		obstacle->Update();
	}
}

int ObstacleManager::RandomLaneIndex() {
	std::uniform_int_distribution<int> distribution(0, kLaneCount - 1);
	return distribution(randomEngine_);
}

float ObstacleManager::RandomSpacing() {
	std::uniform_real_distribution<float> distribution(kMinObstacleSpacing, kMaxObstacleSpacing);
	return distribution(randomEngine_);
}

ObstacleType ObstacleManager::RandomType() {
	// 石灯籠を中心にしつつ、時々ジャンプ用の丸太を混ぜる。
	std::bernoulli_distribution distribution(0.35);
	return distribution(randomEngine_) ? ObstacleType::Log : ObstacleType::StoneLantern;
}

void ObstacleManager::Draw() {
	for (const auto& obstacle : obstacles_) {
		obstacle->Draw();
	}
}

bool ObstacleManager::CheckCollision(const Math::Vector3& playerPosition) const {
	for (const auto& obstacle : obstacles_) {
		const Math::Vector3& obstaclePosition = obstacle->GetPosition();
		const bool isLog = obstacle->GetType() == ObstacleType::Log;
		if (isLog && playerPosition.y > kLogJumpClearHeight) {
			continue;
		}
		const float halfWidth = isLog ? kLogCollisionHalfWidth : kLanternCollisionHalfWidth;
		const float halfDepth = isLog ? kLogCollisionHalfDepth : kLanternCollisionHalfDepth;
		const bool overlapsX = std::abs(playerPosition.x - obstaclePosition.x) <= halfWidth;
		const bool overlapsZ = std::abs(playerPosition.z - obstaclePosition.z) <= halfDepth;
		if (overlapsX && overlapsZ) {
			return true;
		}
	}
	return false;
}
