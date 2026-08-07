#include "Math.h" // 宣言と構造体の定義をインクルード
#include <cmath> // std::tan, std::sin, std::cos のために必要
#include <cassert> // (必要に応じて)

namespace Math {

	// 単位行列
	Matrix4x4 MakeIdentity4x4() {
		Matrix4x4 identity;
		identity.m[0][0] = 1.0f;	identity.m[0][1] = 0.0f;	identity.m[0][2] = 0.0f;	identity.m[0][3] = 0.0f;
		identity.m[1][0] = 0.0f;	identity.m[1][1] = 1.0f;	identity.m[1][2] = 0.0f;	identity.m[1][3] = 0.0f;
		identity.m[2][0] = 0.0f;	identity.m[2][1] = 0.0f;	identity.m[2][2] = 1.0f;	identity.m[2][3] = 0.0f;
		identity.m[3][0] = 0.0f;	identity.m[3][1] = 0.0f;	identity.m[3][2] = 0.0f;	identity.m[3][3] = 1.0f;
		return identity;
	}

	// 4x4の掛け算
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result;
		result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
		result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
		result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
		result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

		result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
		result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
		result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
		result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

		result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
		result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
		result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
		result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

		result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
		result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
		result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
		result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

		return result;
	}

	// X軸で回転
	Matrix4x4 MakeRotateXMatrix(float radian) {
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		return { 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, cosTheta, sinTheta, 0.0f,
				0.0f, -sinTheta, cosTheta, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };
	}

	// Y軸で回転
	Matrix4x4 MakeRotateYMatrix(float radian) {
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		return { cosTheta, 0.0f, -sinTheta, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				sinTheta, 0.0f, cosTheta, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };
	}

	// Z軸で回転
	Matrix4x4 MakeRotateZMatrix(float radian) {
		float cosTheta = std::cos(radian);
		float sinTheta = std::sin(radian);
		return { cosTheta, sinTheta, 0.0f, 0.0f,
				-sinTheta, cosTheta, 0.0f , 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };
	}

	// Affine変換
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
		Matrix4x4 result = Multiply(Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)), MakeRotateZMatrix(rotate.z));
		result.m[0][0] *= scale.x;
		result.m[0][1] *= scale.x;
		result.m[0][2] *= scale.x;

		result.m[1][0] *= scale.y;
		result.m[1][1] *= scale.y;
		result.m[1][2] *= scale.y;

		result.m[2][0] *= scale.z;
		result.m[2][1] *= scale.z;
		result.m[2][2] *= scale.z;

		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		return result;
	}


	// 透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
	{
		float cotHalfFovV = 1.0f / std::tan(fovY / 2.0f);
		return {
			(cotHalfFovV / aspectRatio), 0.0f, 0.0f, 0.0f,
			0.0f, cotHalfFovV, 0.0f, 0.0f,
			0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
			0.0f, 0.0f, -(nearClip * farClip) / (farClip - nearClip), 0.0f
		};
	}

	// 逆行列 (詳細な実装は main.cpp から移動)
	Matrix4x4 Inverse(const Matrix4x4& m) {
		float determinant = +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
			- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
			- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

		Matrix4x4 result;
		float recpDeterminant = 1.0f / determinant;
		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
			m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
			m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
			m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
			m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
		result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
			m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
			m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

		result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
			m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
			m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
			m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
			m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
			m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) * recpDeterminant;
		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
			m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
			m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) * recpDeterminant;

		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
			m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] -
			m.m[0][1] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
			m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
			m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) * recpDeterminant;
		result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) * recpDeterminant;

		result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
			m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
			m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
			m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
			m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
			m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) * recpDeterminant;
		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
			m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) * recpDeterminant;

		return result;
	}

	// 平行投影行列
	Matrix4x4 MakeOrthorgraphicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
	{
		return{
			2.0f / (right - left),0.0f,0.0f,0.0f,
			0.0f,2.0f / (top - bottom),0.0f,0.0f,
			0.0f,0.0f,1.0f / (farClip - nearClip),0.0f,
			(left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1.0f,

		};
	}

	Math::Vector3 Math::Lerp(const Vector3& v1, const Vector3& v2, float t)
	{
		return {
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t,
			v1.z + (v2.z - v1.z) * t
		};
	}

	float Math::Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
	}

	Math::Quaternion Math::Normalize(const Quaternion& q)
	{
		float length = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

		if (length == 0.0f) {
			return { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		return {
			q.x / length,
			q.y / length,
			q.z / length,
			q.w / length
		};
	}

	Math::Quaternion Math::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		Quaternion q2Temp = q2;

		float dot = Dot(q1, q2Temp);

		if (dot < 0.0f) {
			q2Temp.x = -q2Temp.x;
			q2Temp.y = -q2Temp.y;
			q2Temp.z = -q2Temp.z;
			q2Temp.w = -q2Temp.w;
			dot = -dot;
		}

		if (dot >= 1.0f - 0.0005f) {
			Quaternion result{
				q1.x + (q2Temp.x - q1.x) * t,
				q1.y + (q2Temp.y - q1.y) * t,
				q1.z + (q2Temp.z - q1.z) * t,
				q1.w + (q2Temp.w - q1.w) * t
			};

			return Normalize(result);
		}

		float theta = std::acos(dot);
		float sinTheta = std::sin(theta);

		float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
		float scale1 = std::sin(t * theta) / sinTheta;

		Quaternion result{
			q1.x * scale0 + q2Temp.x * scale1,
			q1.y * scale0 + q2Temp.y * scale1,
			q1.z * scale0 + q2Temp.z * scale1,
			q1.w * scale0 + q2Temp.w * scale1
		};

		return Normalize(result);
	}

	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
	{
		Quaternion q = Normalize(quaternion);

		float x = q.x;
		float y = q.y;
		float z = q.z;
		float w = q.w;

		Matrix4x4 result = MakeIdentity4x4();

		result.m[0][0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
		result.m[0][1] = 2.0f * x * y + 2.0f * w * z;
		result.m[0][2] = 2.0f * x * z - 2.0f * w * y;

		result.m[1][0] = 2.0f * x * y - 2.0f * w * z;
		result.m[1][1] = 1.0f - 2.0f * x * x - 2.0f * z * z;
		result.m[1][2] = 2.0f * y * z + 2.0f * w * x;

		result.m[2][0] = 2.0f * x * z + 2.0f * w * y;
		result.m[2][1] = 2.0f * y * z - 2.0f * w * x;
		result.m[2][2] = 1.0f - 2.0f * x * x - 2.0f * y * y;

		return result;
	}

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
	{
		Matrix4x4 result = MakeRotateMatrix(rotate);

		result.m[0][0] *= scale.x;
		result.m[0][1] *= scale.x;
		result.m[0][2] *= scale.x;

		result.m[1][0] *= scale.y;
		result.m[1][1] *= scale.y;
		result.m[1][2] *= scale.y;

		result.m[2][0] *= scale.z;
		result.m[2][1] *= scale.z;
		result.m[2][2] *= scale.z;

		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;

		return result;
	}

	Matrix4x4 Transpose(const Matrix4x4& m)
	{
		Matrix4x4 result{};

		for (int row = 0; row < 4; ++row) {
			for (int column = 0; column < 4; ++column) {
				result.m[row][column] = m.m[column][row];
			}
		}

		return result;
	}

} // namespace Math の終わり
