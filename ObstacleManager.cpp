#include "ObstacleManager.h"
#include "Obstacle.h"
#include <algorithm>

namespace {
constexpr int kObstacleCount = 5;
constexpr float kFirstObstacleZ = 20.0f;
constexpr float kObstacleSpacing = 15.0f;
constexpr float kObstacleY = 0.6f;
constexpr float kRecycleOffset = 2.0f;
constexpr float kLanePositions[] = { -2.0f, 0.0f, 2.0f };
constexpr int kLaneCount = 3;
}

ObstacleManager::ObstacleManager() = default;
ObstacleManager::~ObstacleManager() = default;

void ObstacleManager::Initialize(Object3dCommon* object3dCommon) {
	obstacles_.reserve(kObstacleCount);

	for (int i = 0; i < kObstacleCount; ++i) {
		const float z = kFirstObstacleZ + kObstacleSpacing * i;
		const float x = kLanePositions[i % kLaneCount];

		auto obstacle = std::make_unique<Obstacle>();
		obstacle->Initialize(object3dCommon, { x, kObstacleY, z });
		obstacles_.push_back(std::move(obstacle));
	}

	nextLaneIndex_ = kObstacleCount % kLaneCount;
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
			furthestZ += kObstacleSpacing;
			position.x = kLanePositions[nextLaneIndex_];
			position.y = kObstacleY;
			position.z = furthestZ;
			nextLaneIndex_ = (nextLaneIndex_ + 1) % kLaneCount;
			obstacle->SetPosition(position);
		}
		obstacle->Update();
	}
}

void ObstacleManager::Draw() {
	for (const auto& obstacle : obstacles_) {
		obstacle->Draw();
	}
}
