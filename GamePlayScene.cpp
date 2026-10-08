#include "GamePlayScene.h"
#include "Camera.h"
#include "Object3dCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Player.h"
#include "CourseManager.h"
#include "ObstacleManager.h"
#include "GameOverScene.h"
#include "SceneManager.h"
#include <algorithm>
#include <cmath>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

GamePlayScene::GamePlayScene() = default;
GamePlayScene::~GamePlayScene() = default;

void GamePlayScene::Initialize() {
	object3dCommon_ = Object3dCommon::GetInstance();
	soundData_ = Sound::GetInstance()->LoadFile("resources/Alarm01.wav");
	Sound::GetInstance()->PlayWave(soundData_);
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("resources/human/white.png");
	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.3f, 0, 0 });
	camera_->SetTranslate({ 0, 4, -10 });
	object3dCommon_->SetDefaultCamera(camera_.get());
	for (uint32_t i = 0; i < 3; ++i) {
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(SpriteCommon::GetInstance(), "resources/uvChecker.png");
		sprite->SetPosition({ 40.0f + 40.0f * i, 40.0f });
		sprite->SetSize({ 28.0f, 28.0f });
		sprite->SetColor({ 1.0f, 0.15f, 0.15f, 1.0f });
		hpSprites_.push_back(std::move(sprite));
	}
	// モデルのロード
	ModelManager::GetInstance()->LoadModel("human/Mannequin_Large.glb");
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("AnimatedCube.gltf");
	animatedModel_ = ModelManager::GetInstance()->FindModel("human/Mannequin_Large.glb");
	player_ = std::make_unique<Player>();
	player_->Initialize(object3dCommon_, input_);
	courseManager_ = std::make_unique<CourseManager>();
	courseManager_->Initialize(object3dCommon_);
	obstacleManager_ = std::make_unique<ObstacleManager>();
	obstacleManager_->Initialize(object3dCommon_);
	animation_ = LoadAnimationFile("resources/human", "Rig_Large_MovementBasic.glb");
	InitializeSceneReveal();
}

void GamePlayScene::Finalize() {
	player_->Finalize();
	courseManager_->Finalize();
	obstacleManager_->Finalize();
	player_.reset(); courseManager_.reset(); obstacleManager_.reset();
	hpSprites_.clear();
	revealLeftSprite_.reset();
	revealRightSprite_.reset();
	camera_.reset();
	Sound::GetInstance()->Unload(&soundData_);
}

void GamePlayScene::Update() {
#ifdef USE_IMGUI
	player_->DrawImGui();
	courseManager_->DrawImGui();

	ImGui::Begin("Camera Settings");
	Math::Vector3 cameraPosition = camera_->GetTranslate();
	if (ImGui::DragFloat3("Position", &cameraPosition.x, 0.1f)) { camera_->SetTranslate(cameraPosition); }
	Math::Vector3 cameraRotation = camera_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &cameraRotation.x, 0.01f)) { camera_->SetRotate(cameraRotation); }
	ImGui::End();
#endif
	if (animatedModel_ && animation_.duration > 0) { animationTime_ = std::fmod(animationTime_ + 1.0f / 60.0f, animation_.duration); animatedModel_->UpdateSkeleton(animation_, animationTime_); }
	camera_->Update();
	player_->Update();
	courseManager_->Update(camera_->GetTranslate().z);
	obstacleManager_->Update(camera_->GetTranslate().z, courseManager_->GetMoveSpeed());
	if (!player_->IsInvincible() && obstacleManager_->CheckCollision(player_->GetPosition())) {
		player_->OnCollision();
		if (player_->IsDead()) {
			sceneManager_->SetNextScene(std::make_unique<GameOverScene>());
			return;
		}
	}
	for (const auto& sprite : hpSprites_) { sprite->Update(); }
	UpdateSceneReveal();
}

void GamePlayScene::InitializeSceneReveal() {
	revealLeftSprite_ = std::make_unique<Sprite>();
	revealLeftSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	revealLeftSprite_->SetPosition({ 320.0f, 360.0f });
	revealLeftSprite_->SetSize({ 640.0f, 720.0f });
	revealLeftSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	revealRightSprite_ = std::make_unique<Sprite>();
	revealRightSprite_->Initialize(SpriteCommon::GetInstance(), "resources/human/white.png");
	revealRightSprite_->SetPosition({ 960.0f, 360.0f });
	revealRightSprite_->SetSize({ 640.0f, 720.0f });
	revealRightSprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	sceneRevealTime_ = 0.0f;
	sceneRevealActive_ = true;
	revealLeftSprite_->Update();
	revealRightSprite_->Update();
}

void GamePlayScene::UpdateSceneReveal() {
	if (!sceneRevealActive_) {
		return;
	}

	constexpr float kDeltaTime = 1.0f / 60.0f;
	constexpr float kRevealDuration = 0.85f;
	sceneRevealTime_ += kDeltaTime;
	const float t = std::clamp(sceneRevealTime_ / kRevealDuration, 0.0f, 1.0f);
	const float eased = t * t * (3.0f - 2.0f * t);

	revealLeftSprite_->SetPosition({ 320.0f - 650.0f * eased, 360.0f });
	revealRightSprite_->SetPosition({ 960.0f + 650.0f * eased, 360.0f });
	revealLeftSprite_->Update();
	revealRightSprite_->Update();

	if (t >= 1.0f) {
		sceneRevealActive_ = false;
	}
}

void GamePlayScene::Draw() {
	courseManager_->Draw();
	obstacleManager_->Draw();
	player_->Draw();

	ID3D12GraphicsCommandList* commandList = Object3dCommon::GetInstance()->GetDxCommon()->GetCommandList();
	SpriteCommon::GetInstance()->PreDraw(commandList);
	for (int i = 0; i < player_->GetHp(); ++i) {
		hpSprites_[i]->Draw(commandList);
	}
	if (sceneRevealActive_) {
		revealLeftSprite_->Draw(commandList);
		revealRightSprite_->Draw(commandList);
	}
}
