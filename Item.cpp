#include "Item.h"

#include "Object3d.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"
#include <cmath>

namespace {
constexpr float kHalfPi = 1.5707963f;
}

Item::Item() = default;
Item::~Item() = default;

void Item::Initialize(Object3dCommon* object3dCommon, ItemType type, const Math::Vector3& position) {
	type_ = type;
	basePosition_ = position;
	soulParticleTime_ = 0.0f;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);

	if (type_ == ItemType::HealOhuda) {
		object3d_->SetModel("healOhuda.obj");
		object3d_->SetScale({ 0.55f, 0.55f, 0.55f });
	} else {
		object3d_->SetModel("miniSoul.obj");
		// テンプルランのコインのように、連続取得する小さな魂にする。
		object3d_->SetScale({ 0.40f, 0.40f, 0.40f });
	}
	object3d_->SetTranslate(basePosition_);
	// Blenderで上方向だったZ軸を、ゲーム内の上方向Y軸へ向ける。
	object3d_->SetRotate({ -kHalfPi, 0.0f, 0.0f });
	object3d_->Update();
}

void Item::Finalize() {
	object3d_.reset();
}

void Item::Update(float deltaTime, float scrollSpeed) {
	animationTime_ += deltaTime;
	Math::Vector3 position = basePosition_;
	position.y += std::sin(animationTime_ * 3.0f) * 0.12f;
	object3d_->SetTranslate(position);
	// 向きは固定し、上下移動だけで取得物らしさを出す。
	object3d_->SetRotate({ 0.0, 3.0f, 0.0f });
	object3d_->Update();

	// 魂の周囲から青い火を短い間隔で出し、魂にまとわり付く見た目にする。
	if (type_ == ItemType::MiniSoul) {
		soulParticleTime_ += deltaTime;
		if (soulParticleTime_ >= 0.10f) {
			Math::Vector3 firePosition = position;
			firePosition.y += 0.05f;
			// コースの移動量を秒単位へ変換し、火の粉にも引き継がせる。
			ParticleManager::GetInstance()->Emit(
				"soulFire", firePosition, 1, { 0.0f, 0.0f, -scrollSpeed * 60.0f });
			soulParticleTime_ -= 0.10f;
		}
	}
}

void Item::Draw() {
	object3d_->Draw();
}

void Item::SetPosition(const Math::Vector3& position) {
	basePosition_ = position;
}
