#include "Camera.h"
#include "WinApp.h"

// ==============================
// コンストラクタ
// Cameraを作った時の初期値を設定する
// ==============================
Camera::Camera()
{
	// ==============================
	// カメラのTransform初期値
	// scaleは1、rotateは0、translateは少し後ろ
	// ==============================
	transform = {
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -10.0f }
	};

	// ==============================
	// プロジェクション用の初期値
	// ==============================
	fovY = 0.45f;
	aspectRatio =
		float(WinApp::kClientWidth) / float(WinApp::kClientHeight);
	nearClip = 0.1f;
	farClip = 100.0f;

	// ==============================
	// 初期行列を作っておく
	// ==============================
	worldMatrix = Math::MakeAffineMatrix(
		transform.scale,
		transform.rotate,
		transform.translate
	);

	viewMatrix = Math::Inverse(worldMatrix);

	projectionMatrix = Math::MakePerspectiveFovMatrix(
		fovY,
		aspectRatio,
		nearClip,
		farClip
	);

	viewProjectionMatrix = Math::Multiply(
		viewMatrix,
		projectionMatrix
	);
}
// ==============================
// 毎フレーム行列を更新する
// ==============================
void Camera::Update()
{
	// ==============================
	// カメラのTransformからワールド行列を作る
	// ==============================
	worldMatrix = Math::MakeAffineMatrix(
		transform.scale,
		transform.rotate,
		transform.translate
	);

	// ==============================
	// カメラのワールド行列の逆行列がビュー行列
	// ==============================
	viewMatrix = Math::Inverse(worldMatrix);

	// ==============================
	// プロジェクション行列を作る
	// ==============================
	projectionMatrix = Math::MakePerspectiveFovMatrix(
		fovY,
		aspectRatio,
		nearClip,
		farClip
	);

	// ==============================
	// ビュー行列とプロジェクション行列を合成する
	// ==============================
	viewProjectionMatrix = Math::Multiply(
		viewMatrix,
		projectionMatrix
	);
}

// ==============================
// カメラの回転を設定する
// ==============================
void Camera::SetRotate(const Math::Vector3& rotate)
{
	transform.rotate = rotate;
}

// ==============================
// カメラの座標を設定する
// ==============================
void Camera::SetTranslate(const Math::Vector3& translate)
{
	transform.translate = translate;
}

// ==============================
// 垂直方向視野角を設定する
// ==============================
void Camera::SetFovY(float fovY)
{
	this->fovY = fovY;
}

// ==============================
// アスペクト比を設定する
// ==============================
void Camera::SetAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
}

// ==============================
// ニアクリップ距離を設定する
// ==============================
void Camera::SetNearClip(float nearClip)
{
	this->nearClip = nearClip;
}

// ==============================
// ファークリップ距離を設定する
// ==============================
void Camera::SetFarClip(float farClip)
{
	this->farClip = farClip;
}


// ==============================
// カメラのワールド行列を取得する
// ==============================
const Math::Matrix4x4& Camera::GetWorldMatrix() const
{
	return worldMatrix;
}

// ==============================
// ビュー行列を取得する
// ==============================
const Math::Matrix4x4& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

// ==============================
// プロジェクション行列を取得する
// ==============================
const Math::Matrix4x4& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

// ==============================
// ビュー × プロジェクション行列を取得する
// 描画では主にこれを使う
// ==============================
const Math::Matrix4x4& Camera::GetViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}

// ==============================
// カメラの回転を取得する
// ==============================
const Math::Vector3& Camera::GetRotate() const
{
	return transform.rotate;
}

// ==============================
// カメラの座標を取得する
// ==============================
const Math::Vector3& Camera::GetTranslate() const
{
	return transform.translate;
}