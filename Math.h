#pragma once

#include <cmath> // std::cos, std::sin, std::tan のために必要
#include <cstdint> // int32_t のために必要

namespace Math {

	// 構造体の定義
	struct Matrix4x4 {
		float m[4][4];
	};

	struct Vector4 {
		float x; float y; float z; float w;
	};

	struct Vector3 {
		float x; float y; float z;
	};

	struct Vector2 {
		float x; float y;
	};

	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};


	struct Quaternion {
		float x;
		float y;
		float z;
		float w;
	};

	struct QuaternionTransform {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};

	// 数学関数の前方宣言
	// グローバル関数は namespace で囲むことが推奨されるため、
	// Mathという名前空間で宣言します。

	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	Quaternion Normalize(const Quaternion& q);
	float Dot(const Quaternion& q1, const Quaternion& q2);
	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);


	Matrix4x4 MakeIdentity4x4();
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	Matrix4x4 MakeRotateXMatrix(float radian);
	Matrix4x4 MakeRotateYMatrix(float radian);
	Matrix4x4 MakeRotateZMatrix(float radian);
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	Matrix4x4 Inverse(const Matrix4x4& m);
	Matrix4x4 MakeOrthorgraphicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	Matrix4x4 Transpose(const Matrix4x4& m);
}