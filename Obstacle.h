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
	// 横倒しになり切るまでは、立っている丸太に当たり判定を持たせない。
	bool IsReadyForCollision() const;

private:
	std::unique_ptr<Object3d> object3d_;
	ObstacleType type_ = ObstacleType::StoneLantern;
	bool usesFallingLogEntrance_ = false;
	bool logFallStarted_ = false;
	// 倒れ切ったときの着地土煙は、丸太1本につき一度だけ出す。
	bool logLandingDustEmitted_ = false;
	float logFallProgress_ = 0.0f;
	float logStandingSideX_ = 0.0f;
	float logStandingRotationZ_ = 0.0f;
	void ConfigureModel();
};
