#include "Player.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Input.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace {
constexpr int kLaneCount = 3;
constexpr float kLanePositions[] = { -2.0f, 0.0f, 2.0f };
constexpr int kInvincibleFrames = 120;
constexpr int kBlinkIntervalFrames = 6;
constexpr int kInitialHp = 3;
}

Player::Player() = default;
Player::~Player() = default;

void Player::Initialize(Object3dCommon* object3dCommon, Input* input) {
	input_ = input;
	laneIndex_ = 1;
	// シーンに入る前から押されていたキーは、新しい入力として扱わない。
	previousLeftPressed_ = input_->Pushkey(DIK_A);
	previousRightPressed_ = input_->Pushkey(DIK_D);
	invincibleTimer_ = 0;
	isVisible_ = true;
	hp_ = kInitialHp;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetModel("human/Mannequin_Large.glb");
	// Mannequin_Largeは以前のモデルより約2.4倍大きいため縮小する。
	object3d_->SetScale({ 0.4f, 0.4f, 0.4f });
	object3d_->SetTranslate({ 0, 0, 0 });
	object3d_->SetRotate({ 0, 3.14f, 0 });
}

void Player::Finalize() {
	object3d_.reset();
	input_ = nullptr;
}

// プレイヤーの更新処理
void Player::Update() {
	// 無敵状態の処理
	if (invincibleTimer_ > 0) {
		--invincibleTimer_;
		isVisible_ = ((invincibleTimer_ / kBlinkIntervalFrames) % 2) == 0;
	} else {
		isVisible_ = true;
	}

	// 入力処理
	Math::Vector3 position = object3d_->GetTranslate();
	// 左右の入力を取得する。押している間はtrueになる。
	const bool leftPressed = input_->Pushkey(DIK_A);
	const bool rightPressed = input_->Pushkey(DIK_D);
	// 押した瞬間だけ反応するようにするため、前回の状態と比較してトリガーを判定する。
	const bool triggerLeft = leftPressed && !previousLeftPressed_;
	const bool triggerRight = rightPressed && !previousRightPressed_;
	// 前回の状態を更新する。
	previousLeftPressed_ = leftPressed;
	previousRightPressed_ = rightPressed;

	bool acceptInput = true;
#ifdef USE_IMGUI
	acceptInput = !ImGui::GetIO().WantCaptureKeyboard;
#endif
	// 同時押しは移動せず、押した瞬間だけ隣のレーンを選ぶ。
	if (acceptInput && leftPressed != rightPressed) {
		if (triggerLeft) {
			--laneIndex_;
		}
		if (triggerRight) {
			++laneIndex_;
		}
	}
	// レーンの範囲を制限する。
	laneIndex_ = std::clamp(laneIndex_, 0, kLaneCount - 1);

	// 目標まで一定速度で移動する。残り距離が小さければ目標に止める。
	const float targetX = kLanePositions[laneIndex_];
	position.x += std::clamp(targetX - position.x, -moveSpeed_, moveSpeed_);

	object3d_->SetTranslate(position);
	object3d_->Update();
}

void Player::Draw() {
	if (isVisible_) {
		object3d_->Draw();
	}
}

void Player::OnCollision() {
	if (!IsInvincible()) {
		if (hp_ > 0) {
			--hp_;
		}
		invincibleTimer_ = kInvincibleFrames;
		isVisible_ = false;
	}
}

// プレイヤーの位置を取得する
const Math::Vector3& Player::GetPosition() const {
	return object3d_->GetTranslate();
}

void Player::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Player Settings");

	Math::Vector3 position = object3d_->GetTranslate();
	ImGui::Text("Position X: %.2f", position.x);
	bool positionChanged = ImGui::DragFloat("Position Y", &position.y, 0.1f);
	positionChanged |= ImGui::DragFloat("Position Z", &position.z, 0.1f);
	if (positionChanged) {
		object3d_->SetTranslate(position);
	}
	ImGui::Combo("Target Lane", &laneIndex_, "Left\0Center\0Right\0");
	ImGui::Text("Target X: %.2f", kLanePositions[laneIndex_]);
	ImGui::Text("Invincible: %s", IsInvincible() ? "true" : "false");
	ImGui::Text("Invincible Timer: %d", invincibleTimer_);
	ImGui::Text("HP: %d", hp_);

	Math::Vector3 rotation = object3d_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f)) {
		object3d_->SetRotate(rotation);
	}

	Math::Vector3 scale = object3d_->GetScale();
	float uniformScale = scale.x;
	if (ImGui::DragFloat("Uniform Scale", &uniformScale, 0.01f, 0.01f, 5.0f)) {
		uniformScale = std::clamp(uniformScale, 0.01f, 5.0f);
		scale = { uniformScale, uniformScale, uniformScale };
		object3d_->SetScale(scale);
	}
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) {
		object3d_->SetScale(scale);
	}

	ImGui::SeparatorText("Player Lighting");
	Object3d::DirectionalLight& light = object3d_->GetDirectionalLight();
	ImGui::ColorEdit4("Light Color", &light.color.x);
	ImGui::DragFloat3("Light Direction", &light.direction.x, 0.01f, -1.0f, 1.0f);
	if (ImGui::DragFloat("Light Intensity", &light.intensity, 0.01f, 0.0f, 5.0f)) {
		light.intensity = std::clamp(light.intensity, 0.0f, 5.0f);
	}
	if (ImGui::DragFloat("Ambient Intensity", &light.ambientIntensity, 0.01f, 0.0f, 1.0f)) {
		light.ambientIntensity = std::clamp(light.ambientIntensity, 0.0f, 1.0f);
	}
	if (ImGui::Button("Reset Player Lighting")) {
		light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		light.direction = { 0.5f, -1.0f, 0.5f };
		light.intensity = 1.0f;
		light.ambientIntensity = 0.25f;
	}

	if (ImGui::DragFloat("Lane Move Speed", &moveSpeed_, 0.01f, 0.01f, 1.0f)) {
		moveSpeed_ = std::clamp(moveSpeed_, 0.01f, 1.0f);
	}

	ImGui::End();
#endif
}
