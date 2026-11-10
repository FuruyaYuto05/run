#include "ItemManager.h"

#include "Object3dCommon.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr int kItemCount = 12;
constexpr int kSoulTrailLength = 5;
constexpr float kFirstItemZ = 22.0f;
constexpr float kSoulSpacing = 2.25f;
constexpr float kMinLargeGap = 8.0f;
constexpr float kMaxLargeGap = 14.0f;
constexpr float kRecycleOffset = 2.0f;
constexpr float kLanePositions[] = { -2.0f, 0.0f, 2.0f };
constexpr int kLaneCount = 3;
constexpr float kCollectionHalfWidth = 0.72f;
constexpr float kCollectionHalfDepth = 0.82f;

float GetItemY(ItemType type) {
	return type == ItemType::HealOhuda ? 1.45f : 1.10f;
}
}

void ItemManager::Initialize(Object3dCommon* object3dCommon) {
	items_.reserve(kItemCount);
	float furthestZ = kFirstItemZ - kMinLargeGap;
	for (int i = 0; i < kItemCount; ++i) {
		// 回復お札は最後の1個だけにして、魂の列よりかなりレアにする。
		const ItemType type = (i == kItemCount - 1)
			? ItemType::HealOhuda : ItemType::MiniSoul;
		const Math::Vector3 position = NextSpawnPosition(type, furthestZ);
		furthestZ = position.z;
		auto item = std::make_unique<Item>();
		item->Initialize(object3dCommon, type, position);
		items_.push_back(std::move(item));
	}
}

void ItemManager::Finalize() {
	for (const auto& item : items_) {
		item->Finalize();
	}
	items_.clear();
}

void ItemManager::Update(float cameraZ, float moveSpeed, float deltaTime) {
	if (items_.empty()) {
		return;
	}
	for (const auto& item : items_) {
		Math::Vector3 position = item->GetPosition();
		position.z -= moveSpeed;
		item->SetPosition(position);
	}

	float furthestZ = items_.front()->GetPosition().z;
	for (const auto& item : items_) {
		if (item->GetPosition().z > furthestZ) {
			furthestZ = item->GetPosition().z;
		}
	}
	for (const auto& item : items_) {
		if (item->GetPosition().z < cameraZ - kRecycleOffset) {
			RespawnItem(*item, furthestZ);
			furthestZ = item->GetPosition().z;
		}
		item->Update(deltaTime);
	}
}

void ItemManager::Draw() {
	for (const auto& item : items_) {
		item->Draw();
	}
}

ItemType ItemManager::CheckCollection(const Math::Vector3& playerPosition) {
	for (const auto& item : items_) {
		const Math::Vector3& position = item->GetPosition();
		if (std::abs(playerPosition.x - position.x) > kCollectionHalfWidth ||
			std::abs(playerPosition.z - position.z) > kCollectionHalfDepth) {
			continue;
		}

		float furthestZ = position.z;
		for (const auto& other : items_) {
			if (other->GetPosition().z > furthestZ) {
				furthestZ = other->GetPosition().z;
			}
		}
		const ItemType collectedType = item->GetType();
		RespawnItem(*item, furthestZ);
		return collectedType;
	}
	return ItemType::None;
}

void ItemManager::RespawnItem(Item& item, float z) {
	item.SetPosition(NextSpawnPosition(item.GetType(), z));
	// 取得・再配置したフレームから新しい位置を描画へ反映する。
	item.Update(0.0f);
}

int ItemManager::RandomLaneIndex() {
	std::uniform_int_distribution<int> distribution(0, kLaneCount - 1);
	return distribution(randomEngine_);
}

float ItemManager::RandomLargeGap() {
	std::uniform_real_distribution<float> distribution(kMinLargeGap, kMaxLargeGap);
	return distribution(randomEngine_);
}

Math::Vector3 ItemManager::NextSpawnPosition(ItemType type, float furthestZ) {
	float nextZ = furthestZ;
	int laneIndex = RandomLaneIndex();

	if (type == ItemType::MiniSoul) {
		if (soulsRemainingInTrail_ <= 0) {
			soulTrailLaneIndex_ = RandomLaneIndex();
			soulsRemainingInTrail_ = kSoulTrailLength;
			nextZ += RandomLargeGap();
		} else {
			nextZ += kSoulSpacing;
		}
		laneIndex = soulTrailLaneIndex_;
		--soulsRemainingInTrail_;
	} else {
		nextZ += RandomLargeGap();
		// お札の次は、次の魂列を別レーンから始める。
		soulsRemainingInTrail_ = 0;
	}

	return { kLanePositions[laneIndex], GetItemY(type), nextZ };
}
