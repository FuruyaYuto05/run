#pragma once

#include <memory>
#include <random>
#include <vector>

class Object3d;
class Object3dCommon;

// 道の外側の木を管理し、通り過ぎた木を奥へ戻して繰り返し使う。
class ForestManager {
public:
	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed);
	void Draw();

private:
	struct Tree {
		std::unique_ptr<Object3d> object;
		float side = 1.0f;
	};

	float RandomSpacing();
	float RandomSideX(float side);
	float RandomRotation();

	std::vector<Tree> trees_;
	std::mt19937 randomEngine_{ std::random_device{}() };
};
