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
	void Update(float cameraZ);
	void Draw();
	void DrawImGui();

private:
	std::vector<std::unique_ptr<CourseSegment>> courseSegments_;
	float moveSpeed_ = 0.1f;
};
