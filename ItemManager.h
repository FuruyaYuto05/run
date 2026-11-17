#pragma once

#include "Item.h"
#include "Math.h"
#include <memory>
#include <random>
#include <vector>

class Object3dCommon;

class ItemManager {
public:
	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed, float deltaTime);
	void Draw();
	ItemType CheckCollection(const Math::Vector3& playerPosition);

private:
	void RespawnItem(Item& item, float z);
	int RandomLaneIndex();
	float RandomLargeGap();
	Math::Vector3 NextSpawnPosition(ItemType type, float furthestZ);

	std::vector<std::unique_ptr<Item>> items_;
	std::mt19937 randomEngine_{ std::random_device{}() };
	int soulTrailLaneIndex_ = 0;
	int soulsRemainingInTrail_ = 0;
};
