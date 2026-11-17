#include "ForestManager.h"

#include "Object3d.h"
#include "Object3dCommon.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
// 森の木の数
constexpr int kTreesPerSide = 15;
constexpr float kFirstTreeZ = -2.0f;
constexpr float kTreeRecycleOffset = 4.0f;
constexpr float kTreeGroundOffsetY = 1.15f;
// 集落は点在させず、参道の左右を家並みとして密に埋める。
constexpr int kBuildingsPerSide = 12;
constexpr float kBuildingRecycleOffset = 6.0f;
constexpr float kHiddenBackgroundZ = -1000.0f;
// 村の家は表示開始よりさらに奥から流し、突然見えないようにする。
constexpr float kVillagePreloadDistance = 90.0f;
constexpr float kGameplayCameraZ = -10.0f;
constexpr float kToriiArrivalDistance = 18.0f;
constexpr float kToriiScale = 0.85f;
// 画面に入り得る範囲だけ描画する。配置自体は奥まで動かし続ける。
constexpr float kDrawBehindDistance = 8.0f;
constexpr float kDrawAheadDistance = 110.0f;

float BuildingRotationTowardRoad(float side) {
	constexpr float kHalfPi = 1.5707963f;
	// モデルの正面を参道へ向ける。左側は右向き、右側は左向きで固定する。
	return side < 0.0f ? kHalfPi : -kHalfPi;
}
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
			tree.object->SetScale({ treeScale_, treeScale_, treeScale_ });
			tree.object->SetRotate({ 0.0f, RandomRotation(), 0.0f });
			tree.object->SetTranslate({ RandomSideX(side), kTreeGroundOffsetY, z });
			tree.object->Update();
			tree.side = side;
			trees_.push_back(std::move(tree));
			z += RandomSpacing();
		}
	}

	// 家は開始時から見えない遠方で流しておく。
	// 村に入る頃に地平線側から現れるため、配置した瞬間に見えない。
	villageBuildings_.reserve(kBuildingsPerSide * 2);
	for (const float side : { -1.0f, 1.0f }) {
		for (int i = 0; i < kBuildingsPerSide; ++i) {
			VillageBuilding building{};
			building.object = std::make_unique<Object3d>();
			building.object->Initialize(object3dCommon);
			building.object->SetModel(i % 2 == 0 ? "building/k_machiya_a.glb" : "building/k_minato_banya.glb");
			building.object->SetScale({ buildingScale_, buildingScale_, buildingScale_ });
			building.object->SetRotate({ 0.0f, BuildingRotationTowardRoad(side), 0.0f });
			const float z = kGameplayCameraZ + villageStartDistance_ +
				villageFirstBuildingDistance_ + kVillagePreloadDistance +
				static_cast<float>(i) * buildingSpacing_;
			building.object->SetTranslate({ RandomBuildingX(side), 0.0f, z });
			building.object->Update();
			building.side = side;
			villageBuildings_.push_back(std::move(building));
		}
	}

	// 鳥居も最初から遠方で流し、300m付近で村の入口として前方へ現す。
	villageTorii_ = std::make_unique<Object3d>();
	villageTorii_->Initialize(object3dCommon);
	villageTorii_->SetModel("building/k_v_ne_torii.glb");
	villageTorii_->SetScale({ kToriiScale, kToriiScale, kToriiScale });
	villageTorii_->SetTranslate({
		0.0f,
		0.0f,
		kGameplayCameraZ + villageStartDistance_ + kToriiArrivalDistance,
	});
	villageTorii_->Update();
}

void ForestManager::Finalize() {
	villageTorii_.reset();
	villageBuildings_.clear();
	trees_.clear();
	villageStarted_ = false;
}

void ForestManager::Update(float cameraZ, float moveSpeed, float runDistance) {
	lastCameraZ_ = cameraZ;
	DrawImGui(runDistance, cameraZ);
	if (trees_.empty()) {
		return;
	}
	if (!villageStarted_ && runDistance >= villageStartDistance_) {
		StartVillage(cameraZ, false);
	}

	float furthestZ = trees_.front().object->GetTranslate().z;
	for (const Tree& tree : trees_) {
		if (tree.object->GetTranslate().z > furthestZ) {
			furthestZ = tree.object->GetTranslate().z;
		}
	}

	for (Tree& tree : trees_) {
		Math::Vector3 position = tree.object->GetTranslate();
		tree.object->SetScale({ treeScale_, treeScale_, treeScale_ });
		position.z -= moveSpeed;
		if (position.z < cameraZ - kTreeRecycleOffset) {
			// 村に入った後は木を再利用せず、通り過ぎた木から自然に減らす。
			if (villageStarted_) {
				position.z = kHiddenBackgroundZ;
			} else {
				furthestZ += RandomSpacing();
				position.x = RandomSideX(tree.side);
				position.z = furthestZ;
				tree.object->SetRotate({ 0.0f, RandomRotation(), 0.0f });
			}
		}
		tree.object->SetTranslate(position);
		tree.object->Update();
	}

	// 村に入る前から建物も移動させる。遠方から連続して近づいてくる。
	float furthestBuildingZ = cameraZ;
	for (const VillageBuilding& building : villageBuildings_) {
		furthestBuildingZ = (std::max)(furthestBuildingZ, building.object->GetTranslate().z);
	}
	for (VillageBuilding& building : villageBuildings_) {
		Math::Vector3 position = building.object->GetTranslate();
		position.z -= moveSpeed;
		if (villageStarted_ && position.z < cameraZ - kBuildingRecycleOffset) {
			furthestBuildingZ += buildingSpacing_;
			position.x = RandomBuildingX(building.side);
			position.z = furthestBuildingZ;
			building.object->SetRotate({ 0.0f, BuildingRotationTowardRoad(building.side), 0.0f });
		}
		building.object->SetScale({ buildingScale_, buildingScale_, buildingScale_ });
		building.object->SetTranslate(position);
		building.object->Update();
	}

	if (villageTorii_) {
		Math::Vector3 position = villageTorii_->GetTranslate();
		position.z -= moveSpeed;
		if (position.z < cameraZ - kBuildingRecycleOffset) {
			position.z = kHiddenBackgroundZ;
		}
		villageTorii_->SetTranslate(position);
		villageTorii_->Update();
	}
}

void ForestManager::Draw() {
	drawnTreeCount_ = 0;
	drawnBuildingCount_ = 0;
	for (const Tree& tree : trees_) {
		if (IsInDrawRange(*tree.object)) {
			tree.object->Draw();
			++drawnTreeCount_;
		}
	}
	if (villageTorii_ && IsInDrawRange(*villageTorii_)) {
		villageTorii_->Draw();
	}
	// 開始時から遠方の家も描画する。視界に入る距離まで来るまでは見えない。
	for (const VillageBuilding& building : villageBuildings_) {
		if (IsInDrawRange(*building.object)) {
			building.object->Draw();
			++drawnBuildingCount_;
		}
	}
}

bool ForestManager::IsInDrawRange(const Object3d& object) const {
	const float z = object.GetTranslate().z;
	return z >= lastCameraZ_ - kDrawBehindDistance &&
		z <= lastCameraZ_ + kDrawAheadDistance;
}

float ForestManager::RandomSpacing() {
	std::uniform_real_distribution<float> distribution(treeSpacingMin_, treeSpacingMax_);
	return distribution(randomEngine_);
}

float ForestManager::RandomSideX(float side) {
	std::uniform_real_distribution<float> distribution(treeSideMin_, treeSideMax_);
	return side * distribution(randomEngine_);
}

float ForestManager::RandomRotation() {
	std::uniform_real_distribution<float> distribution(-0.30f, 0.30f);
	return distribution(randomEngine_);
}

float ForestManager::RandomBuildingX(float side) {
	std::uniform_real_distribution<float> distribution(buildingSideMin_, buildingSideMax_);
	return side * distribution(randomEngine_);
}

void ForestManager::StartVillage(float cameraZ, bool placeImmediately) {
	villageStarted_ = true;
	if (!placeImmediately) {
		return;
	}
	// ImGuiのプレビュー時だけは、確認しやすい距離へ並べ直す。
	for (size_t i = 0; i < villageBuildings_.size(); ++i) {
		VillageBuilding& building = villageBuildings_[i];
		// 左右で同じ列番号を使い、一本道の両脇に家が連なる形にする。
		const size_t row = i % kBuildingsPerSide;
		const float z = cameraZ + villageFirstBuildingDistance_ +
			static_cast<float>(row) * buildingSpacing_;
		building.object->SetTranslate({ RandomBuildingX(building.side), 0.0f, z });
		building.object->SetScale({ buildingScale_, buildingScale_, buildingScale_ });
		building.object->SetRotate({ 0.0f, BuildingRotationTowardRoad(building.side), 0.0f });
		building.object->Update();
	}
}

void ForestManager::DrawImGui(float runDistance, float cameraZ) {
#ifdef USE_IMGUI
	ImGui::Begin("Side Scenery Editor");
	ImGui::Text("Run Distance: %.0f m", runDistance);
	ImGui::Text("Use sliders only. Restart the scene to reset placement.");
	ImGui::Text("Drawn: Trees %zu / %zu, Houses %zu / %zu",
		drawnTreeCount_, trees_.size(), drawnBuildingCount_, villageBuildings_.size());

	ImGui::SeparatorText("Forest / Trees");
	ImGui::SliderFloat("Tree Scale", &treeScale_, 0.05f, 0.60f);
	ImGui::SliderFloat("Tree Side Min", &treeSideMin_, 3.5f, 12.0f);
	ImGui::SliderFloat("Tree Side Max", &treeSideMax_, 3.5f, 12.0f);
	ImGui::SliderFloat("Tree Spacing Min", &treeSpacingMin_, 1.0f, 15.0f);
	ImGui::SliderFloat("Tree Spacing Max", &treeSpacingMax_, 1.0f, 15.0f);

	ImGui::SeparatorText("Village / Buildings");
	ImGui::SliderFloat("Village Start (m)", &villageStartDistance_, 0.0f, 1000.0f);
	ImGui::SliderFloat("House Scale", &buildingScale_, 0.05f, 1.50f);
	ImGui::SliderFloat("House Side Min", &buildingSideMin_, 4.3f, 15.0f);
	ImGui::SliderFloat("House Side Max", &buildingSideMax_, 4.3f, 15.0f);
	ImGui::SliderFloat("House Spacing", &buildingSpacing_, 2.0f, 30.0f);
	ImGui::SliderFloat("First House Distance", &villageFirstBuildingDistance_, 5.0f, 100.0f);
	ImGui::Text("Torii entrance: 300m");
	if (!villageStarted_ && ImGui::Button("Preview Village Now")) {
		StartVillage(cameraZ, true);
	}
	ImGui::SameLine();
	ImGui::Text("Village: %s", villageStarted_ ? "Active" : "Waiting");

	// 逆転した範囲のまま乱数を作らないように補正する。
	if (treeSideMin_ > treeSideMax_) { treeSideMax_ = treeSideMin_; }
	if (treeSpacingMin_ > treeSpacingMax_) { treeSpacingMax_ = treeSpacingMin_; }
	if (buildingSideMin_ > buildingSideMax_) { buildingSideMax_ = buildingSideMin_; }
	ImGui::End();
#else
	(void)runDistance;
	(void)cameraZ;
#endif
}
