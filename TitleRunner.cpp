#include "TitleRunner.h"

#include "Camera.h"
#include "Model.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include <cmath>

namespace {
constexpr float kDeltaTime = 1.0f / 60.0f;
constexpr float kHalfPi = 1.5707963f;
constexpr char kRunnerModelPath[] = "human/walk.gltf";
}

TitleRunner::TitleRunner() = default;
TitleRunner::~TitleRunner() = default;

void TitleRunner::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	ModelManager::GetInstance()->LoadModel(kRunnerModelPath);
	model_ = ModelManager::GetInstance()->FindModel(kRunnerModelPath);
	animation_ = LoadAnimationFile("resources/human", "walk.gltf");
	animationTime_ = 0.0f;

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetCamera(camera);
	object3d_->SetModel(model_);
	object3d_->SetScale({ 0.9f, 0.9f, 0.9f });
	object3d_->SetRotate({ 0.0f, kHalfPi, 0.0f });
	object3d_->SetTranslate({ startX_, 0.0f, 2.0f });
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

	if (model_ && animation_.duration > 0.0f) {
		animationTime_ = std::fmod(animationTime_ + kDeltaTime, animation_.duration);
		model_->UpdateSkeleton(animation_, animationTime_);
	}

	Math::Vector3 position = object3d_->GetTranslate();
	position.x += moveSpeed_;
	if (position.x > endX_) {
		position.x = startX_;
	}
	object3d_->SetTranslate(position);
	object3d_->Update();
}

void TitleRunner::Draw() {
	if (object3d_) {
		object3d_->Draw();
	}
}
