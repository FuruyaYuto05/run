#pragma once

#include "BaseScene.h"
#include "Animation.h"
#include "Sound.h"
#include <vector>
#include <memory>

class Camera;
class ParticleEmitter;
class Object3d;
class Object3dCommon;
class Model;
class Sprite;

class GamePlayScene : public BaseScene {
public:
	GamePlayScene();
	~GamePlayScene() override;
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	Object3dCommon* object3dCommon_ = nullptr;
	SoundData soundData_{};
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<ParticleEmitter> particleEmitter_;
	std::unique_ptr<Object3d> object3d1_;
	std::unique_ptr<Object3d> object3d2_;
	Model* animatedModel_ = nullptr;
	std::unique_ptr<Sprite> sprite_;
	std::vector<std::unique_ptr<Sprite>> sprites_;
	Animation animation_{};
	float animationTime_ = 0.0f;
};
