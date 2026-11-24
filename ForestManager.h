#pragma once

#include <memory>
#include <random>
#include <vector>

class Object3d;
class Object3dCommon;

// 道の外側の木と、途中から現れる村の建物を管理する背景クラス。
class ForestManager {
public:
	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed, float runDistance);
	void Draw();
	void DrawImGui(float runDistance, float cameraZ);

private:
	struct Tree {
		std::unique_ptr<Object3d> object;
		float side = 1.0f;
	};
	struct VillageBuilding {
		std::unique_ptr<Object3d> object;
		float side = 1.0f;
	};

	float RandomSpacing();
	float RandomSideX(float side);
	float RandomRotation();
	float RandomBuildingX(float side);
	void StartVillage(float cameraZ, bool placeImmediately);
	bool IsInDrawRange(const Object3d& object) const;

	std::vector<Tree> trees_;
	std::vector<VillageBuilding> villageBuildings_;
	std::unique_ptr<Object3d> villageTorii_;
	bool villageStarted_ = false;
	// ImGuiから即時調整する背景配置パラメーター。
	float treeScale_ = 0.25f;
	float treeSideMin_ = 3.803f;
	float treeSideMax_ = 3.803f;
	float treeSpacingMin_ = 2.383f;
	float treeSpacingMax_ = 15.0f;
	float villageStartDistance_ = 300.0f;
	float villageFirstBuildingDistance_ = 20.898f;
	float buildingScale_ = 0.651f;
	float buildingSideMin_ = 4.3f;
	float buildingSideMax_ = 4.3f;
	float buildingSpacing_ = 2.0f;
	float lastCameraZ_ = -10.0f;
	size_t drawnTreeCount_ = 0;
	size_t drawnBuildingCount_ = 0;
	std::mt19937 randomEngine_{ std::random_device{}() };
};
