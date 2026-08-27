#include "CourseManager.h"
#include "CourseSegment.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
constexpr int kCourseSegmentCount = 5;
constexpr float kCourseSegmentLength = 12.0f;
constexpr float kCourseSegmentHalfLength = kCourseSegmentLength * 0.5f;
constexpr float kFirstCourseSegmentZ = 5.0f;
}

CourseManager::CourseManager() = default;
CourseManager::~CourseManager() = default;

void CourseManager::Initialize(Object3dCommon* object3dCommon) {
	courseSegments_.reserve(kCourseSegmentCount);
	for (int i = 0; i < kCourseSegmentCount; ++i) {
		const float z = kFirstCourseSegmentZ + kCourseSegmentLength * i;
		auto courseSegment = std::make_unique<CourseSegment>();
		courseSegment->Initialize(object3dCommon, { 0.0f, 0.0f, z });
		courseSegments_.push_back(std::move(courseSegment));
	}
}

void CourseManager::Finalize() {
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Finalize();
	}
	courseSegments_.clear();
}

void CourseManager::Update(float cameraZ) {
	if (courseSegments_.empty()) {
		return;
	}

	for (const auto& courseSegment : courseSegments_) {
		Math::Vector3 position = courseSegment->GetPosition();
		position.z -= moveSpeed_;
		courseSegment->SetPosition(position);
	}

	float furthestZ = courseSegments_.front()->GetPosition().z;
	for (const auto& courseSegment : courseSegments_) {
		furthestZ = std::max(furthestZ, courseSegment->GetPosition().z);
	}

	for (const auto& courseSegment : courseSegments_) {
		Math::Vector3 position = courseSegment->GetPosition();
		if (position.z + kCourseSegmentHalfLength < cameraZ) {
			furthestZ += kCourseSegmentLength;
			position.z = furthestZ;
			courseSegment->SetPosition(position);
		}
		courseSegment->Update();
	}
}

void CourseManager::Draw() {
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Draw();
	}
}

void CourseManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Course Settings");
	ImGui::DragFloat("Move Speed", &moveSpeed_, 0.01f, 0.0f, 10.0f);
	if (moveSpeed_ < 0.0f) {
		moveSpeed_ = 0.0f;
	}
	ImGui::End();
#endif
}
