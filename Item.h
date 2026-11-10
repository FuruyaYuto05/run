#pragma once

#include "Math.h"
#include <memory>

class Object3d;
class Object3dCommon;

enum class ItemType {
	None,
	MiniSoul,
	HealOhuda,
};

class Item {
public:
	Item();
	~Item();

	void Initialize(Object3dCommon* object3dCommon, ItemType type, const Math::Vector3& position);
	void Finalize();
	void Update(float deltaTime);
	void Draw();

	ItemType GetType() const { return type_; }
	const Math::Vector3& GetPosition() const { return basePosition_; }
	void SetPosition(const Math::Vector3& position);

private:
	std::unique_ptr<Object3d> object3d_;
	ItemType type_ = ItemType::None;
	Math::Vector3 basePosition_{};
	float animationTime_ = 0.0f;
};
