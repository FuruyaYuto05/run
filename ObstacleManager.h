#pragma once

#include <memory>
#include <vector>

class Obstacle;
class Object3dCommon;

class ObstacleManager {
public:
	ObstacleManager();
	~ObstacleManager();

	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, float moveSpeed);
	void Draw();

private:
	std::vector<std::unique_ptr<Obstacle>> obstacles_;
	int nextLaneIndex_ = 0;
};
