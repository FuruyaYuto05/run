#include "CourseSegment.h"
#include "Object3d.h"
#include "Object3dCommon.h"

CourseSegment::CourseSegment() = default;
CourseSegment::~CourseSegment() = default;

void CourseSegment::Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition) {
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetModel("plane.obj");

	// plane.objを横幅5、奥行き12程度の仮足場として使用する
	object3d_->SetScale({ 2.5f, 6.0f, 1.0f });
	object3d_->SetRotate({ -1.5707963f, 0.0f, 0.0f });
	object3d_->SetTranslate(initialPosition);
}

void CourseSegment::Finalize() {
	object3d_.reset();
}

void CourseSegment::Update() {
	object3d_->Update();
}

void CourseSegment::Draw() {
	object3d_->Draw();
}

const Math::Vector3& CourseSegment::GetPosition() const {
	return object3d_->GetTranslate();
}

void CourseSegment::SetPosition(const Math::Vector3& position) {
	object3d_->SetTranslate(position);
}
