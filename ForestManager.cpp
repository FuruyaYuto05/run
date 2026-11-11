#include "ForestManager.h"

#include "Object3d.h"
#include "Object3dCommon.h"

namespace {
// 森の木の数
constexpr int kTreesPerSide = 15;
constexpr float kFirstTreeZ = -2.0f;
// 森の木の間隔
constexpr float kMinTreeSpacing = 2.0f;
// 森の木の間隔の最大値
constexpr float kMaxTreeSpacing = 6.0f;
constexpr float kTreeRecycleOffset = 4.0f;
// tree.obj は高さ約29なので、ゲーム内では高さ約7へ縮小する。
constexpr float kTreeScale = 0.25f;
constexpr float kTreeGroundOffsetY = 1.15f;
}

void ForestManager::Initialize(Object3dCommon* object3dCommon) {
	trees_.reserve(kTreesPerSide * 2);
	for (const float side : { -1.0f, 1.0f }) {
		float z = kFirstTreeZ;
		for (int i = 0; i < kTreesPerSide; ++i) {
			Tree tree{};
			tree.object = std::make_unique<Object3d>();
			tree.object->Initialize(object3dCommon);
			tree.object->SetModel("tree.obj");
			tree.object->SetScale({ kTreeScale, kTreeScale, kTreeScale });
			tree.object->SetRotate({ 0.0f, RandomRotation(), 0.0f });
			tree.object->SetTranslate({ RandomSideX(side), kTreeGroundOffsetY, z });
			tree.object->Update();
			tree.side = side;
			trees_.push_back(std::move(tree));
			z += RandomSpacing();
		}
	}
}

void ForestManager::Finalize() {
	trees_.clear();
}

void ForestManager::Update(float cameraZ, float moveSpeed) {
	if (trees_.empty()) {
		return;
	}

	float furthestZ = trees_.front().object->GetTranslate().z;
	for (const Tree& tree : trees_) {
		if (tree.object->GetTranslate().z > furthestZ) {
			furthestZ = tree.object->GetTranslate().z;
		}
	}

	for (Tree& tree : trees_) {
		Math::Vector3 position = tree.object->GetTranslate();
		position.z -= moveSpeed;
		if (position.z < cameraZ - kTreeRecycleOffset) {
			furthestZ += RandomSpacing();
			position.x = RandomSideX(tree.side);
			position.z = furthestZ;
			tree.object->SetRotate({ 0.0f, RandomRotation(), 0.0f });
		}
		tree.object->SetTranslate(position);
		tree.object->Update();
	}
}

void ForestManager::Draw() {
	for (const Tree& tree : trees_) {
		tree.object->Draw();
	}
}

float ForestManager::RandomSpacing() {
	std::uniform_real_distribution<float> distribution(kMinTreeSpacing, kMaxTreeSpacing);
	return distribution(randomEngine_);
}

float ForestManager::RandomSideX(float side) {
	// 道端より少し外側に置く。木の半径を含めても道をふさがない位置。
	std::uniform_real_distribution<float> distribution(4.3f, 6.0f);
	return side * distribution(randomEngine_);
}

float ForestManager::RandomRotation() {
	std::uniform_real_distribution<float> distribution(-0.30f, 0.30f);
	return distribution(randomEngine_);
}
