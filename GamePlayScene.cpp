#include "GamePlayScene.h"
#include "Camera.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Model.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include <cmath>
#include <imgui.h>

GamePlayScene::GamePlayScene() = default;
GamePlayScene::~GamePlayScene() = default;

void GamePlayScene::Initialize() {
	object3dCommon_ = Object3dCommon::GetInstance();
	soundData_ = Sound::GetInstance()->LoadFile("resources/Alarm01.wav");
	Sound::GetInstance()->PlayWave(soundData_);
	ParticleManager::GetInstance()->CreateParticleGroup("circle", "resources/effect/circle2.png");
	particleEmitter_ = std::make_unique<ParticleEmitter>("circle", Math::Vector3{ 0, 0, 0 }, 10, 0.5f);
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
	ModelManager::GetInstance()->LoadModel("multiMaterial.obj");
	animatedModel_ = ModelManager::GetInstance()->FindModel("human/sneakWalk.gltf");
	object3d1_ = std::make_unique<Object3d>();
	object3d1_->Initialize(object3dCommon_);
	object3d1_->SetModel("human/sneakWalk.gltf");
	object3d1_->SetTranslate({ -2, 0, 0 });
	object3d1_->SetRotate({ 0, 3.14f, 0 });
	object3d2_ = std::make_unique<Object3d>();
	object3d2_->Initialize(object3dCommon_);
	object3d2_->SetModel("multiMaterial.obj");
	object3d2_->SetTranslate({ 2, 0, 0 });
	animation_ = LoadAnimationFile("resources/human", "sneakWalk.gltf");
}

void GamePlayScene::Finalize() {
	object3d1_.reset(); object3d2_.reset(); sprite_.reset();
	sprites_.clear();
	particleEmitter_.reset(); camera_.reset();
	Sound::GetInstance()->Unload(&soundData_);
}

void GamePlayScene::Update() {
#ifdef USE_IMGUI
	ImGui::Begin("Object3d_2 Settings");
	Math::Vector3 position = object3d2_->GetTranslate();
	if (ImGui::DragFloat3("Position", &position.x, 0.1f)) { object3d2_->SetTranslate(position); }
	Math::Vector3 rotation = object3d2_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f)) { object3d2_->SetRotate(rotation); }
	Math::Vector3 scale = object3d2_->GetScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) { object3d2_->SetScale(scale); }
	ImGui::End();

	ImGui::Begin("Lighting Settings");
	Object3d::DirectionalLight& light = object3d2_->GetDirectionalLight();
	ImGui::DragFloat3("Direction", &light.direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::ColorEdit4("Light Color", &light.color.x);
	ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, 10.0f);
	if (Model* model = object3d2_->GetModel()) {
		float shininess = model->GetShininess();
		if (ImGui::DragFloat("Shininess", &shininess, 1.0f, 1.0f, 256.0f)) {
			model->SetShininess(shininess);
		}
	}
	ImGui::End();

	ImGui::Begin("Camera Settings");
	Math::Vector3 cameraPosition = camera_->GetTranslate();
	if (ImGui::DragFloat3("Position", &cameraPosition.x, 0.1f)) { camera_->SetTranslate(cameraPosition); }
	Math::Vector3 cameraRotation = camera_->GetRotate();
	if (ImGui::DragFloat3("Rotation", &cameraRotation.x, 0.01f)) { camera_->SetRotate(cameraRotation); }
	ImGui::End();
#endif
	if (animatedModel_ && animation_.duration > 0) { animationTime_ = std::fmod(animationTime_ + 1.0f / 60.0f, animation_.duration); animatedModel_->UpdateSkeleton(animation_, animationTime_); }
	camera_->Update();
	ParticleManager::GetInstance()->Update(camera_->GetViewProjectionMatrix());
	particleEmitter_->Update(); object3d1_->Update(); object3d2_->Update(); sprite_->Update();
	for (const auto& sprite : sprites_) { sprite->Update(); }
}

void GamePlayScene::Draw() {
	object3d2_->Draw();
	ParticleManager::GetInstance()->Draw();
}
