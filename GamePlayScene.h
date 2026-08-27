#pragma once

#include "BaseScene.h"
#include "Animation.h"
#include "Sound.h"
#include <vector>
#include <memory>

class Camera;
class Object3dCommon;
class Model;
class Sprite;
class Player;
class CourseManager;

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
	std::unique_ptr<Player> player_;
	std::unique_ptr<CourseManager> courseManager_;
	Model* animatedModel_ = nullptr;
	std::unique_ptr<Sprite> sprite_;
	std::vector<std::unique_ptr<Sprite>> sprites_;
	Animation animation_{};
	float animationTime_ = 0.0f;
};
