#include "Obstacle.h"
#include "Object3d.h"
#include "Object3dCommon.h"

Obstacle::Obstacle() = default;
Obstacle::~Obstacle() = default;

void Obstacle::Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition, ObstacleType type) {
	type_ = type;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	ConfigureModel();
	object3d_->SetTranslate(initialPosition);
}

void Obstacle::ConfigureModel() {
	if (type_ == ObstacleType::Log) {
		// 道を横断する丸太。低いのでジャンプで越えられる。
		object3d_->SetModel("maruta.obj");
		object3d_->SetScale({ 0.55f, 0.55f, 0.55f });
	} else {
		object3d_->SetModel("tyoutin.obj");
		object3d_->SetScale({ 0.65f, 0.65f, 0.65f });
	}
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

void Obstacle::SetType(ObstacleType type) {
	if (type_ == type) {
		return;
	}
	type_ = type;
	ConfigureModel();
}
