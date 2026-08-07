#include "ParticleEmitter.h"

#include "ParticleManager.h"

ParticleEmitter::ParticleEmitter(
    const std::string& particleGroupName,
    const Math::Vector3& position,
    uint32_t emitCount,
    float frequency
)
{
    // ==============================
    // 発生させるグループ名
    // ==============================
    particleGroupName_ = particleGroupName;

    // ==============================
    // 発生位置
    // ==============================
    position_ = position;

    // ==============================
    // 1回で発生させる数
    // ==============================
    emitCount_ = emitCount;

    // ==============================
    // 発生間隔
    // ==============================
    frequency_ = frequency;

    // ==============================
    // 時刻初期化
    // ==============================
    frequencyTime_ = 0.0f;
}

void ParticleEmitter::Update()
{
    // ==============================
    // 時間を進める
    // ==============================
    const float kDeltaTime = 1.0f / 60.0f;
    frequencyTime_ += kDeltaTime;

    // ==============================
    // 発生間隔を超えたらEmit
    // ==============================
    if (frequencyTime_ >= frequency_) {

        Emit();

        // ==============================
        // 余った時間を残す
        // 例: 0.51秒たまったら 0.5秒ぶん引いて0.01秒残す
        // ==============================
        frequencyTime_ -= frequency_;
    }
}


void ParticleEmitter::Emit()
{
    // ==============================
    // ParticleManagerに発生依頼
    // ==============================
    ParticleManager::GetInstance()->Emit(
        particleGroupName_,
        position_,
        emitCount_
    );
}