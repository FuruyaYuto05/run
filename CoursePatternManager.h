#pragma once

#include "Item.h"
#include "Math.h"
#include "Obstacle.h"
#include <memory>
#include <random>
#include <vector>

class Object3dCommon;

// 障害物と取得物を別々に抽選せず、遊び方のある一組の配置として管理する。
class CoursePatternManager {
public:
	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed, float deltaTime);
	void Draw();
	bool CheckCollision(const Math::Vector3& playerPosition) const;
	ItemType CheckCollection(const Math::Vector3& playerPosition);

private:
	enum class PatternType {
		SoulPair,
		LanternDetour,
		LogReward,
		OfudaDetour,
	};

	void AppendPattern();
	void AddObstacle(ObstacleType type, float x, float z);
	void AddItem(ItemType type, int laneIndex, float z);
	PatternType ChoosePattern();
	int RandomLaneIndex();
	int RandomDifferentLane(int excludedLane);
	void RemovePassedObjects(float cameraZ);

	Object3dCommon* object3dCommon_ = nullptr;
	std::vector<std::unique_ptr<Obstacle>> obstacles_;
	std::vector<std::unique_ptr<Item>> items_;
	std::mt19937 randomEngine_{ std::random_device{}() };
	float nextPatternZ_ = 20.0f;
	PatternType previousPattern_ = PatternType::SoulPair;
	bool forceSoulPairAfterLog_ = false;
};
