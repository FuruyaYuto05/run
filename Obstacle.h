#pragma once

#include "Math.h"
#include <memory>

class Object3d;
class Object3dCommon;

enum class ObstacleType {
	StoneLantern,
	Log,
};

class Obstacle {
public:
	Obstacle();
	~Obstacle();

	void Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition, ObstacleType type);
	void Finalize();
	void Update();
	void Draw();

	const Math::Vector3& GetPosition() const;
	void SetPosition(const Math::Vector3& position);
	void SetType(ObstacleType type);
	ObstacleType GetType() const { return type_; }

private:
	std::unique_ptr<Object3d> object3d_;
	ObstacleType type_ = ObstacleType::StoneLantern;
	void ConfigureModel();
};
