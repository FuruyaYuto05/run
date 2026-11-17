#include "CourseManager.h"
#include "CourseSegment.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif
// コースセグメントの数と長さを定義
namespace {
// コースセグメントの数
constexpr int kCourseSegmentCount = 8;
// 追加した3枚はスタート位置より後方へ配置する
constexpr int kRearCourseSegmentCount = 3;
// コースセグメントの長さ
constexpr float kCourseSegmentLength = 12.0f;
// コースセグメントの半分の長さ
constexpr float kCourseSegmentHalfLength = kCourseSegmentLength * 0.5f;
// 最初のコースセグメントのZ座標
constexpr float kFirstCourseSegmentZ = 5.0f;
constexpr float kDeltaTime = 1.0f / 60.0f;
// 15秒ごとに少しずつ速くし、慣れてきた頃に緊張感を上げる。
constexpr float kSpeedUpIntervalSeconds = 15.0f;
constexpr float kSpeedUpAmount = 0.02f;
constexpr float kMaxMoveSpeed = 0.30f;
}

CourseManager::CourseManager() = default;
CourseManager::~CourseManager() = default;

void CourseManager::Initialize(Object3dCommon* object3dCommon) {
	courseSegments_.reserve(kCourseSegmentCount);
	for (int i = 0; i < kCourseSegmentCount; ++i) {
		const float z = kFirstCourseSegmentZ +
			kCourseSegmentLength * (i - kRearCourseSegmentCount);
		auto courseSegment = std::make_unique<CourseSegment>();
		courseSegment->Initialize(object3dCommon, { 0.0f, 0.0f, z });
		courseSegments_.push_back(std::move(courseSegment));
	}
	comparisonElapsedTime_ = 0.0f;
	runDistance_ = 0.0f;
	moveSpeed_ = initialMoveSpeed_;
	totalRecycleCount_ = 0;
}

void CourseManager::Finalize() {
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Finalize();
	}
	courseSegments_.clear();
}

void CourseManager::Update(float cameraZ, bool recycleSegments) {
	// コースセグメントが存在しない場合は何もしない
	if (courseSegments_.empty()) {
		return;
	}
	// コースセグメントを更新
	for (const auto& courseSegment : courseSegments_) {
		Math::Vector3 position = courseSegment->GetPosition();
		position.z -= moveSpeed_;
		courseSegment->SetPosition(position);
	}
	if (recycleSegments) {
		comparisonElapsedTime_ += kDeltaTime;
		const int speedLevel = static_cast<int>(comparisonElapsedTime_ / kSpeedUpIntervalSeconds);
		moveSpeed_ = std::min(initialMoveSpeed_ + speedLevel * kSpeedUpAmount, kMaxMoveSpeed);
		// スクロールした床の距離を、プレイ距離として記録する。
		runDistance_ += moveSpeed_;
		// 最も奥にある床の位置を基準に、通過した床を前方へ再配置する。
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
				++totalRecycleCount_;
			}
		}
	}
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Update();
	}
}

void CourseManager::UpdateTransformOnly() {
	// 床を移動させず、現在のカメラを各セグメントの描画行列へ反映する。
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Update();
	}
}

void CourseManager::Draw() {
	// コースセグメントを描画
	for (const auto& courseSegment : courseSegments_) {
		courseSegment->Draw();
	}
}

void CourseManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Course Settings");
	ImGui::DragFloat("Initial Move Speed", &initialMoveSpeed_, 0.01f, 0.0f, kMaxMoveSpeed);
	if (initialMoveSpeed_ < 0.0f) {
		initialMoveSpeed_ = 0.0f;
	}

	const ImGuiIO& io = ImGui::GetIO();
	const float frameTimeMs = io.Framerate > 0.0f ? 1000.0f / io.Framerate : 0.0f;

	ImGui::Separator();
	ImGui::TextColored(
		{ 0.25f, 1.0f, 0.45f, 1.0f },
		"Mode: Floor Reuse");
	ImGui::Text("Elapsed Time: %.1f sec", comparisonElapsedTime_);
	ImGui::Text("Run Distance: %.0f", runDistance_);
	ImGui::Text("Current Speed: %.2f", moveSpeed_);
	ImGui::Text("Initial Segments: %d", kCourseSegmentCount);
	ImGui::Text("Current Segments: %zu", courseSegments_.size());
	ImGui::Text("Newly Generated: 0");
	ImGui::Text("Reuse Count: %zu", totalRecycleCount_);
	ImGui::Text("Drawn Floor Objects: %zu", courseSegments_.size());
	ImGui::Text("FPS: %.1f", io.Framerate);
	ImGui::Text("Frame Time: %.3f ms", frameTimeMs);
	ImGui::End();
#endif
}
