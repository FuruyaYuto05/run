#pragma once

#include "Math.h"
#include <memory>
#include <random>
#include <vector>

class Obstacle;
class Object3dCommon;
enum class ObstacleType;

class ObstacleManager {
public:
	ObstacleManager();
	~ObstacleManager();

	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed);
	void Draw();
	bool CheckCollision(const Math::Vector3& playerPosition) const;

private:
	std::vector<std::unique_ptr<Obstacle>> obstacles_;
	int RandomLaneIndex();
	float RandomSpacing();
	ObstacleType RandomType();

	std::mt19937 randomEngine_{ std::random_device{}() };
};
