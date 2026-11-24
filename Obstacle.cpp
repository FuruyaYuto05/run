#include "Obstacle.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr float kHalfPi = 1.5707963f;
constexpr float kLogStandingY = 2.20f;
constexpr float kLogGroundY = 0.33f;
// 倒れ終わるまでに余裕ができるよう、遠めから反応させる。
constexpr float kLogFallTriggerZ = 16.0f;
constexpr float kLogFallDuration = 0.52f;

float SmoothStep(float t) {
	return t * t * (3.0f - 2.0f * t);
}
}

Obstacle::Obstacle() = default;
Obstacle::~Obstacle() = default;

void Obstacle::Initialize(Object3dCommon* object3dCommon, const Math::Vector3& initialPosition, ObstacleType type) {
	type_ = type;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	ConfigureModel();
	object3d_->SetTranslate(initialPosition);
	// コース端に置かれた丸太だけは、最初は縦に立てておく。
	// 自機が近づくと参道側へ倒れ、道を横断する障害物になる。
	usesFallingLogEntrance_ = type_ == ObstacleType::Log && std::abs(initialPosition.x) > 3.0f;
	logFallStarted_ = false;
	logLandingDustEmitted_ = false;
	logFallProgress_ = 0.0f;
	if (usesFallingLogEntrance_) {
		logStandingSideX_ = initialPosition.x;
		// 左右とも道側へ倒れる向きにする。
		logStandingRotationZ_ = initialPosition.x < 0.0f ? kHalfPi : -kHalfPi;
		Math::Vector3 standingPosition = initialPosition;
		// 丸太の根元を地面の高さに合わせる。
		standingPosition.y = kLogGroundY + kLogStandingY;
		object3d_->SetTranslate(standingPosition);
		object3d_->SetRotate({ 0.0f, 0.0f, logStandingRotationZ_ });
	} else {
		object3d_->SetRotate({ 0.0f, 0.0f, 0.0f });
	}
}

void Obstacle::ConfigureModel() {
	if (type_ == ObstacleType::Log) {
		// 道を横断する丸太。低いのでジャンプで越えられる。
		object3d_->SetModel("maruta.obj");
		object3d_->SetScale({ 0.55f, 0.55f, 0.55f });
	} else {
		// 森の参道に置く障害物は、沈み灯籠のモデルへ差し替える。
		object3d_->SetModel("building/k_ryu_shizumi_tourou.glb");
		object3d_->SetScale({ 0.80f, 0.80f, 0.80f });
	}
}

void Obstacle::Finalize() {
	object3d_.reset();
}

void Obstacle::Update() {
	if (usesFallingLogEntrance_) {
		Math::Vector3 position = object3d_->GetTranslate();
		if (!logFallStarted_ && position.z <= kLogFallTriggerZ) {
			logFallStarted_ = true;
		}
		if (logFallStarted_) {
			// 根元を道端に固定したまま、上側だけが道へ倒れ込む。
			// 位置を中央まで補間しないことで、滑り込む見た目を防ぐ。
			logFallProgress_ = (std::min)(1.0f, logFallProgress_ + (1.0f / 60.0f) / kLogFallDuration);
			const float eased = SmoothStep(logFallProgress_);
			const float rotationZ = logStandingRotationZ_ * (1.0f - eased);
			const float inwardDirection = logStandingSideX_ < 0.0f ? 1.0f : -1.0f;
			position.x = logStandingSideX_ + inwardDirection * kLogStandingY * std::cos(rotationZ);
			position.y = kLogGroundY + kLogStandingY * std::abs(std::sin(rotationZ));
			object3d_->SetTranslate(position);
			object3d_->SetRotate({ 0.0f, 0.0f, rotationZ });

			// 横倒しになって地面へ着いた瞬間だけ、接地点から土煙を上げる。
			// 毎フレームEmitすると煙が増え続けるため、倒壊ごとに一度だけ発生させる。
			if (!logLandingDustEmitted_ && logFallProgress_ >= 1.0f) {
				// 根元・中央・先端へ分けて出し、丸太全体が地面へぶつかった見た目にする。
				const float rootX = logStandingSideX_;
				const float centerX = position.x;
				const float tipX = logStandingSideX_ + inwardDirection * kLogStandingY * 2.0f;
				const float dustY = kLogGroundY + 0.03f;
				ParticleManager::GetInstance()->Emit("dust", { rootX, dustY, position.z }, 14);
				ParticleManager::GetInstance()->Emit("dust", { centerX, dustY, position.z }, 14);
				ParticleManager::GetInstance()->Emit("dust", { tipX, dustY, position.z }, 14);
				logLandingDustEmitted_ = true;
			}
		}
	}
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

bool Obstacle::IsReadyForCollision() const {
	return !usesFallingLogEntrance_ || logFallProgress_ >= 1.0f;
}
