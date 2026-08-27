#include "Player.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Input.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

Player::Player() = default;
Player::~Player() = default;

void Player::Initialize(Object3dCommon* object3dCommon, Input* input) {
	input_ = input;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetModel("human/sneakWalk.gltf");
	object3d_->SetTranslate({ 0, 0, 0 });
	object3d_->SetRotate({ 0, 3.14f, 0 });
}

void Player::Finalize() {
	object3d_.reset();
	input_ = nullptr;
}

void Player::Update() {
	Math::Vector3 position = object3d_->GetTranslate();

	if (input_->Pushkey(DIK_A)) {
		position.x -= moveSpeed_;
	}
	if (input_->Pushkey(DIK_D)) {
		position.x += moveSpeed_;
	}
	if (moveMinX_ > moveMaxX_) {
		std::swap(moveMinX_, moveMaxX_);
	}
	position.x = std::clamp(position.x, moveMinX_, moveMaxX_);

	object3d_->SetTranslate(position);
	object3d_->Update();
}

void Player::Draw() {
	object3d_->Draw();
}

void Player::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Player Settings");

	Math::Vector3 position = object3d_->GetTranslate();
	if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
		object3d_->SetTranslate(position);
	}

	Math::Vector3 rotation = object3d_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f)) {
		object3d_->SetRotate(rotation);
	}

	Math::Vector3 scale = object3d_->GetScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) {
		object3d_->SetScale(scale);
	}

	ImGui::DragFloat("Move Speed", &moveSpeed_, 0.01f, 0.0f, 1.0f);
	if (ImGui::DragFloat("Move Min X", &moveMinX_, 0.1f)) {
		if (moveMinX_ > moveMaxX_) {
			moveMinX_ = moveMaxX_;
		}
	}
	if (ImGui::DragFloat("Move Max X", &moveMaxX_, 0.1f)) {
		if (moveMaxX_ < moveMinX_) {
			moveMaxX_ = moveMinX_;
		}
	}

	ImGui::End();
#endif
}
