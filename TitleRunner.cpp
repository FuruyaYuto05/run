#include "TitleRunner.h"

#include "Camera.h"
#include "Model.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include <cmath>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kHalfPi = 1.5707963f;
constexpr char kRunnerModelPath[] = "human/Mannequin_Large.glb";
}

TitleRunner::TitleRunner() = default;
TitleRunner::~TitleRunner() = default;

void TitleRunner::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	ModelManager::GetInstance()->LoadModel(kRunnerModelPath);
	model_ = ModelManager::GetInstance()->FindModel(kRunnerModelPath);
	animation_ = LoadAnimationFile("resources/human", "Rig_Large_MovementBasic.glb");
	animationTime_ = 0.0f;

	// Object3dの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetCamera(camera);
	object3d_->SetModel(model_);
	object3d_->SetScale({ 0.9f, 0.9f, 0.9f });
	object3d_->SetRotate({ 0.0f, kHalfPi, 0.0f });
	object3d_->SetTranslate({ startX_, -3.0f, 2.0f });
	object3d_->Update();
}

void TitleRunner::Finalize() {
	object3d_.reset();
	model_ = nullptr;
}

void TitleRunner::Update() {
	if (!object3d_) {
		return;
	}

	// アニメーションの更新
	if (model_ && animation_.duration > 0.0f) {
		animationTime_ = std::fmod(animationTime_ + kDeltaTime, animation_.duration);
		model_->UpdateSkeleton(animation_, animationTime_);
	}

	// 位置の更新
	Math::Vector3 position = object3d_->GetTranslate();
	if (isMoving_) {
		position.x += moveSpeed_;
		if (position.x > endX_) {
			position.x = startX_;
		}
	}
	object3d_->SetTranslate(position);
	object3d_->Update();
}

void TitleRunner::ResetRun() {
	if (!object3d_) {
		return;
	}

	// ImGuiで調整した高さと奥行きは残し、横位置だけ開始地点へ戻す。
	Math::Vector3 position = object3d_->GetTranslate();
	position.x = startX_;
	object3d_->SetTranslate(position);
	isMoving_ = true;

	// 走行モーションも最初の姿勢から再生し直す。
	animationTime_ = 0.0f;
	if (model_ && animation_.duration > 0.0f) {
		model_->UpdateSkeleton(animation_, animationTime_);
	}
	object3d_->Update();
}

Math::Vector3 TitleRunner::GetPosition() const {
	if (!object3d_) {
		return { 0.0f, -3.0f, 2.0f };
	}
	return object3d_->GetTranslate();
}

void TitleRunner::Draw() {
	if (object3d_) {
		object3d_->Draw();
	}
}

void TitleRunner::DrawImGui() {
#ifdef USE_IMGUI
	if (!object3d_) {
		return;
	}

	ImGui::Begin("Title Runner Settings");

	Math::Vector3 position = object3d_->GetTranslate();
	if (ImGui::DragFloat3("Position", &position.x, 0.05f)) {
		object3d_->SetTranslate(position);
	}

	Math::Vector3 scale = object3d_->GetScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 10.0f)) {
		object3d_->SetScale(scale);
	}

	Math::Vector3 rotation = object3d_->GetRotate();
	if (ImGui::DragFloat3("Rotation (radian)", &rotation.x, 0.01f)) {
		object3d_->SetRotate(rotation);
	}

	ImGui::Separator();
	ImGui::Checkbox("Move", &isMoving_);
	ImGui::DragFloat("Move Speed", &moveSpeed_, 0.001f, 0.0f, 0.5f, "%.3f");
	ImGui::DragFloat("Loop Start X", &startX_, 0.1f);
	ImGui::DragFloat("Loop End X", &endX_, 0.1f);

	if (ImGui::Button("Reset Runner")) {
		startX_ = -8.5f;
		endX_ = 8.5f;
		moveSpeed_ = 0.045f;
		object3d_->SetTranslate({ startX_, -3.0f, 2.0f });
		object3d_->SetScale({ 0.9f, 0.9f, 0.9f });
		object3d_->SetRotate({ 0.0f, kHalfPi, 0.0f });
		ResetRun();
	}

	ImGui::TextUnformatted("Position Y: height / Position Z: depth");
	ImGui::End();
#endif
}
