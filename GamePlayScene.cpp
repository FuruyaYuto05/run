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
	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.3f, 0, 0 });
	camera_->SetTranslate({ 0, 4, -10 });
	object3dCommon_->SetDefaultCamera(camera_.get());
	for (uint32_t i = 0; i < 5; ++i) {
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(SpriteCommon::GetInstance(), i % 2 ? "monsterBall.png" : "resources/uvChecker.png");
		sprite->SetPosition({ 100.0f + 150.0f * i, 100.0f });
		sprite->SetSize({ 128, 128 });
		sprites_.push_back(std::move(sprite));
	}
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteCommon::GetInstance(), "resources/uvChecker.png");
	ModelManager::GetInstance()->LoadModel("human/sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("plane.obj");
	animatedModel_ = ModelManager::GetInstance()->FindModel("human/sneakWalk.gltf");
	player_ = std::make_unique<Player>();
	player_->Initialize(object3dCommon_, input_);
	courseManager_ = std::make_unique<CourseManager>();
	courseManager_->Initialize(object3dCommon_);
	animation_ = LoadAnimationFile("resources/human", "sneakWalk.gltf");
}

void GamePlayScene::Finalize() {
	player_->Finalize();
	courseManager_->Finalize();
	player_.reset(); courseManager_.reset(); sprite_.reset();
	sprites_.clear();
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
	player_->Update(); courseManager_->Update(camera_->GetTranslate().z); sprite_->Update();
	for (const auto& sprite : sprites_) { sprite->Update(); }
}

void GamePlayScene::Draw() {
	courseManager_->Draw();
	player_->Draw();
}
