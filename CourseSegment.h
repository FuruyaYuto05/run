#pragma once

#include "Math.h"
#include <memory>

class Object3d;
class Object3dCommon;

class CourseSegment {
public:
	CourseSegment();
	~CourseSegment();

	void Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition);
	void Finalize();
	void Update();
	void Draw();
	const Math::Vector3& GetPosition() const;
	void SetPosition(const Math::Vector3& position);

private:
	std::unique_ptr<Object3d> object3d_;
};
