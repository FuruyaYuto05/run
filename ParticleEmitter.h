#pragma once

#include <string>
#include <cstdint>

#include "Math.h"

class ParticleEmitter
{
public:
    // ==============================
    // コンストラクタ
    // ==============================
    ParticleEmitter(
        const std::string& particleGroupName,
        const Math::Vector3& position,
        uint32_t emitCount,
        float frequency
    );

    // ==============================
    // 更新
    // ==============================
    void Update();

    void SetPosition(const Math::Vector3& position) { position_ = position; }
    const Math::Vector3& GetPosition() const { return position_; }

    // ==============================
    // パーティクル発生
    // ==============================
    void Emit();

private:
    std::string particleGroupName_;
    Math::Vector3 position_{};
    uint32_t emitCount_ = 0;

    // ==============================
    // 発生間隔
    // ==============================
    float frequency_ = 0.5f;

    // ==============================
    // 発生頻度用の時刻
    // ==============================
    float frequencyTime_ = 0.0f;
};