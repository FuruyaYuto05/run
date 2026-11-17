#pragma once

#include <memory>
#include <vector>

class CourseSegment;
class Object3dCommon;

class CourseManager {
public:
	CourseManager();
	~CourseManager();

	void Initialize(Object3dCommon* object3dCommon);
	void Finalize();
	void Update(float cameraZ, bool recycleSegments = true);
	void UpdateTransformOnly();
	void Draw();
	void DrawImGui();
	float GetMoveSpeed() const { return moveSpeed_; }
	float GetRunDistance() const { return runDistance_; }

private:
	std::vector<std::unique_ptr<CourseSegment>> courseSegments_;
	float moveSpeed_ = 0.16f;
	float initialMoveSpeed_ = 0.16f;
	float comparisonElapsedTime_ = 0.0f;
	float runDistance_ = 0.0f;
	size_t totalRecycleCount_ = 0;
};
