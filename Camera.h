#pragma once
#include "Math.h"

// ==============================
// カメラクラス
// ==============================
class Camera
{
public:

	// ==============================
	// コンストラクタ
	// Cameraを作った瞬間に初期値を入れる
	// ==============================
	Camera();

	// ==============================
	// 毎フレーム行列を更新する
	// ==============================
	void Update();

	// ==============================
	// setter
	// 外からCameraの値を変更するための関数
	// ==============================
	void SetRotate(const Math::Vector3& rotate);
	void SetTranslate(const Math::Vector3& translate);
	void SetFovY(float fovY);
	void SetAspectRatio(float aspectRatio);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);

	// ==============================
    // getter
    // Camera内の行列や座標を外から取得する
    // ==============================
	const Math::Matrix4x4& GetWorldMatrix() const;
	const Math::Matrix4x4& GetViewMatrix() const;
	const Math::Matrix4x4& GetProjectionMatrix() const;
	const Math::Matrix4x4& GetViewProjectionMatrix() const;

	const Math::Vector3& GetRotate() const;
	const Math::Vector3& GetTranslate() const;

private:
	// ==============================
	// カメラの位置・回転・拡縮
	// ==============================
	Math::Transform transform;

	// ==============================
	// カメラのワールド行列
	// ==============================
	Math::Matrix4x4 worldMatrix;

	// ==============================
	// ビュー行列
	// ==============================
	Math::Matrix4x4 viewMatrix;

	// ==============================
	// プロジェクション行列
	// ==============================
	Math::Matrix4x4 projectionMatrix;

	// ==============================
	// 垂直方向の視野角
	// ==============================
	float fovY;

	// ==============================
	// アスペクト比
	// 1280 / 720 など
	// ==============================
	float aspectRatio;

	// ==============================
	// ニアクリップ距離
	// これより近いものは描画されない
	// ==============================
	float nearClip;

	// ==============================
	// ファークリップ距離
	// これより遠いものは描画されない
	// ==============================
	float farClip;

	// ==============================
    // ビュー行列とプロジェクション行列を合成した行列
    // 描画時のWVP計算に使う
    // ==============================
	Math::Matrix4x4 viewProjectionMatrix;
};