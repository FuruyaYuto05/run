#include "Obstacle.h"
#include "Object3d.h"
#include "Object3dCommon.h"

Obstacle::Obstacle() = default;
Obstacle::~Obstacle() = default;

void Obstacle::Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition) {
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetModel("AnimatedCube.gltf");
	object3d_->SetScale({ 0.6f, 0.6f, 0.6f });
	object3d_->SetTranslate(initialPosition);
}

void Obstacle::Finalize() {
	object3d_.reset();
}

void Obstacle::Update() {
	object3d_->Update();
}

void Obstacle::Draw() {
	object3d_->Draw();
}

const Math::Vector3& Obstacle::GetPosition() const {
	return object3d_->GetTranslate();
}

void Obstacle::SetPosition(const Math::Vector3& position) {
	object3d_->SetTranslate(position);
}
