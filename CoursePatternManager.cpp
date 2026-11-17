#include "CoursePatternManager.h"

#include "Object3dCommon.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr float kLanePositions[] = { -2.0f, 0.0f, 2.0f };
constexpr int kLaneCount = 3;
constexpr float kLanternY = 0.0f;
constexpr float kLogY = 0.33f;
constexpr float kSoulY = 1.10f;
constexpr float kOfudaY = 1.45f;
constexpr float kKeepAheadDistance = 115.0f;
constexpr float kRemoveBehindDistance = 4.0f;
constexpr float kSoulSpacing = 2.25f;
constexpr float kLanternCollisionHalfWidth = 0.8f;
constexpr float kLanternCollisionHalfDepth = 0.8f;
constexpr float kLogCollisionHalfWidth = 3.4f;
constexpr float kLogCollisionHalfDepth = 0.8f;
constexpr float kLogJumpClearHeight = 0.85f;

float ObstacleY(ObstacleType type) {
	return type == ObstacleType::Log ? kLogY : kLanternY;
}
}

void CoursePatternManager::Initialize(Object3dCommon* object3dCommon) {
	object3dCommon_ = object3dCommon;
	nextPatternZ_ = 20.0f;
	// 開始直後から奥まで連続して見える分を用意する。
	while (nextPatternZ_ < kKeepAheadDistance) {
		AppendPattern();
	}
}

void CoursePatternManager::Finalize() {
	for (const auto& obstacle : obstacles_) {
		obstacle->Finalize();
	}
	for (const auto& item : items_) {
		item->Finalize();
	}
	obstacles_.clear();
	items_.clear();
	object3dCommon_ = nullptr;
}

void CoursePatternManager::Update(float cameraZ, float moveSpeed, float deltaTime) {
	for (const auto& obstacle : obstacles_) {
		Math::Vector3 position = obstacle->GetPosition();
		position.z -= moveSpeed;
		obstacle->SetPosition(position);
		obstacle->Update();
	}
	for (const auto& item : items_) {
		Math::Vector3 position = item->GetPosition();
		position.z -= moveSpeed;
		item->SetPosition(position);
		item->Update(deltaTime);
	}

	nextPatternZ_ -= moveSpeed;
	RemovePassedObjects(cameraZ);
	while (nextPatternZ_ < cameraZ + kKeepAheadDistance) {
		AppendPattern();
	}
}

void CoursePatternManager::Draw() {
	for (const auto& obstacle : obstacles_) {
		obstacle->Draw();
	}
	for (const auto& item : items_) {
		item->Draw();
	}
}

bool CoursePatternManager::CheckCollision(const Math::Vector3& playerPosition) const {
	for (const auto& obstacle : obstacles_) {
		const Math::Vector3& position = obstacle->GetPosition();
		if (!obstacle->IsReadyForCollision()) {
			continue;
		}
		const bool isLog = obstacle->GetType() == ObstacleType::Log;
		if (isLog && playerPosition.y > kLogJumpClearHeight) {
			continue;
		}
		const float halfWidth = isLog ? kLogCollisionHalfWidth : kLanternCollisionHalfWidth;
		const float halfDepth = isLog ? kLogCollisionHalfDepth : kLanternCollisionHalfDepth;
		if (std::abs(playerPosition.x - position.x) <= halfWidth &&
			std::abs(playerPosition.z - position.z) <= halfDepth) {
			return true;
		}
	}
	return false;
}

ItemType CoursePatternManager::CheckCollection(const Math::Vector3& playerPosition) {
	constexpr float kCollectionHalfWidth = 0.72f;
	constexpr float kCollectionHalfDepth = 0.82f;
	for (auto it = items_.begin(); it != items_.end(); ++it) {
		const Math::Vector3& position = (*it)->GetPosition();
		if (std::abs(playerPosition.x - position.x) > kCollectionHalfWidth ||
			std::abs(playerPosition.z - position.z) > kCollectionHalfDepth) {
			continue;
		}
		const ItemType type = (*it)->GetType();
		(*it)->Finalize();
		items_.erase(it);
		return type;
	}
	return ItemType::None;
}

void CoursePatternManager::AppendPattern() {
	const PatternType pattern = ChoosePattern();
	const float z = nextPatternZ_;

	switch (pattern) {
	case PatternType::SoulPair: {
		// 2個の魂は障害物なしで、どのレーンにも自然に置く。
		const int lane = RandomLaneIndex();
		AddItem(ItemType::MiniSoul, lane, z + 2.0f);
		AddItem(ItemType::MiniSoul, lane, z + 2.0f + kSoulSpacing);
		nextPatternZ_ += 11.0f;
		break;
	}
	case PatternType::LanternDetour: {
		// 石灯籠はもっとも多い基本障害物。空いたレーンに魂を2個置く。
		const int blockedLane = RandomLaneIndex();
		const int rewardLane = RandomDifferentLane(blockedLane);
		AddObstacle(ObstacleType::StoneLantern, kLanePositions[blockedLane], z + 3.5f);
		AddItem(ItemType::MiniSoul, rewardLane, z + 6.0f);
		AddItem(ItemType::MiniSoul, rewardLane, z + 6.0f + kSoulSpacing);
		nextPatternZ_ += 14.0f;
		break;
	}
	case PatternType::LogReward: {
		// 低頻度の丸太。飛び越えた先に5個並べて、ジャンプする理由を作る。
		// 丸太は左右どちらかの道の外側で立たせ、近づくと中央へ倒す。
		std::bernoulli_distribution sideDistribution(0.5);
		const float standingSideX = sideDistribution(randomEngine_) ? -4.2f : 4.2f;
		AddObstacle(ObstacleType::Log, standingSideX, z + 3.5f);
		for (int i = 0; i < 5; ++i) {
			AddItem(ItemType::MiniSoul, 1, z + 6.2f + kSoulSpacing * static_cast<float>(i));
		}
		nextPatternZ_ += 19.0f;
		forceSoulPairAfterLog_ = true;
		break;
	}
	case PatternType::OfudaDetour: {
		// 回復お札は障害物の奥、かつ魂の列とは異なるレーンにだけ置く。
		const int blockedLane = RandomLaneIndex();
		const int ofudaLane = RandomDifferentLane(blockedLane);
		AddObstacle(ObstacleType::StoneLantern, kLanePositions[blockedLane], z + 3.5f);
		AddItem(ItemType::HealOhuda, ofudaLane, z + 7.0f);
		nextPatternZ_ += 15.0f;
		break;
	}
	}
	previousPattern_ = pattern;
}

void CoursePatternManager::AddObstacle(ObstacleType type, float x, float z) {
	auto obstacle = std::make_unique<Obstacle>();
	obstacle->Initialize(object3dCommon_, { x, ObstacleY(type), z }, type);
	obstacle->Update();
	obstacles_.push_back(std::move(obstacle));
}

void CoursePatternManager::AddItem(ItemType type, int laneIndex, float z) {
	auto item = std::make_unique<Item>();
	item->Initialize(object3dCommon_, type, { kLanePositions[laneIndex], type == ItemType::HealOhuda ? kOfudaY : kSoulY, z });
	items_.push_back(std::move(item));
}

CoursePatternManager::PatternType CoursePatternManager::ChoosePattern() {
	if (forceSoulPairAfterLog_) {
		forceSoulPairAfterLog_ = false;
		return PatternType::SoulPair;
	}
	// 灯籠70%、通常の魂10%、回復お札13%、丸太7%。
	// お札のパターンにも灯籠を置くため、全体として灯籠が主役になる。
	// 連続で同じ形が続かないよう再抽選する。
	std::discrete_distribution<int> distribution({ 10, 70, 7, 13 });
	PatternType result = previousPattern_;
	for (int attempt = 0; attempt < 4 && result == previousPattern_; ++attempt) {
		result = static_cast<PatternType>(distribution(randomEngine_));
	}
	return result;
}

int CoursePatternManager::RandomLaneIndex() {
	std::uniform_int_distribution<int> distribution(0, kLaneCount - 1);
	return distribution(randomEngine_);
}

int CoursePatternManager::RandomDifferentLane(int excludedLane) {
	std::uniform_int_distribution<int> distribution(0, kLaneCount - 2);
	const int value = distribution(randomEngine_);
	return value >= excludedLane ? value + 1 : value;
}

void CoursePatternManager::RemovePassedObjects(float cameraZ) {
	const float border = cameraZ - kRemoveBehindDistance;
	auto removeObstacle = [border](const std::unique_ptr<Obstacle>& obstacle) {
		if (obstacle->GetPosition().z >= border) { return false; }
		obstacle->Finalize();
		return true;
	};
	auto removeItem = [border](const std::unique_ptr<Item>& item) {
		if (item->GetPosition().z >= border) { return false; }
		item->Finalize();
		return true;
	};
	obstacles_.erase(std::remove_if(obstacles_.begin(), obstacles_.end(), removeObstacle), obstacles_.end());
	items_.erase(std::remove_if(items_.begin(), items_.end(), removeItem), items_.end());
}
